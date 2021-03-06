/*
 * Copyright (c) 2013-2019, Huawei Technologies Co., Ltd. All rights reserved.
 * Copyright (c) 2020, Huawei Device Co., Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "los_signal.h"
#include "pthread.h"
#include "los_process_pri.h"
#include "los_hw_pri.h"
#include "user_copy.h"
#ifdef LOSCFG_SECURITY_CAPABILITY
#include "capability_api.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

int raise(int sig)
{
    (VOID)sig;
    PRINT_ERR("%s NOT SUPPORT\n", __FUNCTION__);
    errno = ENOSYS;
    return -1;
}

#define GETUNMASKSET(procmask, pendFlag) ((~(procmask)) & (sigset_t)(pendFlag))
#define UINT64_BIT_SIZE 64
/****************************************************
用来检测参数signum 代表的信号是否已加入至参数set信号集里。
如果信号集里已有该信号则返回1，否则返回0。如果有错误则返回-1
****************************************************/
int OsSigIsMember(const sigset_t *set, int signo)
{
    int ret = LOS_NOK;
    /* In musl, sig No bits 00000100 present sig No 3, but  1<< 3 = 00001000, so signo needs minus 1 */ 
	//在musl中，sig No bits 00000100表示sig No 3，但是在SIGNO2SET中 1<<3 = 00001000,因此signo需要减1
    signo -= 1;
    /* Verify the signal */
    if (GOOD_SIGNO(signo)) {//有效信号判断
        /* Check if the signal is in the set */
        ret = ((*set & SIGNO2SET((unsigned int)signo)) != 0);//检查信号是否还在集合中
    }

    return ret;
}
//给任务(线程)发送一个信号
int OsTcbDispatch(LosTaskCB *stcb, siginfo_t *info)
{
    bool masked = FALSE;
    sig_cb *sigcb = &stcb->sig;

    OS_RETURN_IF_NULL(sigcb);
    /* If signo is 0, not send signal, just check process or pthread exist */
    if (info->si_signo == 0) {
        return 0;
    }
    masked = (bool)OsSigIsMember(&sigcb->sigprocmask, info->si_signo);//@note_thinking 这里还有 masked= -1的情况要处理!!!
    if (masked) {//集合中已有该信号
        /* If signal is in wait list and mask list, need unblock it */ //如果信号在等待列表和掩码列表中，需要解除阻止
        if (!LOS_ListEmpty(&sigcb->waitList) && OsSigIsMember(&sigcb->sigwaitmask, info->si_signo)) {//waitList上挂的都是task,sigwaitmask表示是否在等si_signo
            OsTaskWake(stcb);//唤醒任务,从阻塞状态变成就绪态
            OsSigEmptySet(&sigcb->sigwaitmask);//将sigwaitmask清空,都已经变成就绪状态了,当然就没有要等待的信号了.
        } else {
            OsSigAddSet(&sigcb->sigPendFlag, info->si_signo);//
        }
    } else {//
        /* unmasked signal actions */
        OsSigAddSet(&sigcb->sigFlag, info->si_signo);
        if (!LOS_ListEmpty(&sigcb->waitList) && OsSigIsMember(&sigcb->sigwaitmask, info->si_signo)) {
            OsTaskWake(stcb);
            OsSigEmptySet(&sigcb->sigwaitmask);
        }
    }
    (void) memcpy_s(&sigcb->sigunbinfo, sizeof(siginfo_t), info, sizeof(siginfo_t));
    return 0;
}

void OsSigMaskSwitch(LosTaskCB * const rtcb, sigset_t set)
{
    sigset_t unmaskset;

    rtcb->sig.sigprocmask = set;
    unmaskset = GETUNMASKSET(rtcb->sig.sigprocmask, rtcb->sig.sigPendFlag);
    if (unmaskset != NULL_SIGNAL_SET) {
        /* pendlist do */
        rtcb->sig.sigFlag |= unmaskset;
        rtcb->sig.sigPendFlag ^= unmaskset;
    }
}
//设置进程的信号掩码
int OsSigprocMask(int how, const sigset_t_l *setl, sigset_t_l *oldset)
{
    LosTaskCB *spcb = NULL;
    sigset_t oldSigprocmask;
    int ret = LOS_OK;
    unsigned int intSave;
    sigset_t set;
    int retVal;

    if (setl != NULL) {
        retVal = LOS_ArchCopyFromUser(&set, &(setl->sig[0]), sizeof(sigset_t));
        if (retVal != 0) {
            return -EFAULT;
        }
    }
    SCHEDULER_LOCK(intSave);
    spcb = OsCurrTaskGet();
    /* If requested, copy the old mask to user. */ //如果需要，请将旧掩码复制给用户
    oldSigprocmask = spcb->sig.sigprocmask;

    /* If requested, modify the current signal mask. */ //如有要求，修改当前信号屏蔽
    if (setl != NULL) {
        /* Okay, determine what we are supposed to do */
        switch (how) {
            /* Set the union of the current set and the signal
             * set pointed to by set as the new sigprocmask.
             */
            case SIG_BLOCK:
                spcb->sig.sigprocmask |= set;//增加信号屏蔽位
                break;
            /* Set the intersection of the current set and the
             * signal set pointed to by set as the new sigprocmask.
             */
            case SIG_UNBLOCK:
                spcb->sig.sigprocmask &= ~(set);//解除信号屏蔽位
                break;
            /* Set the signal set pointed to by set as the new sigprocmask. */
            case SIG_SETMASK:
                spcb->sig.sigprocmask = set;//设置一个新的屏蔽掩码
                break;
            default:
                ret = -EINVAL;
                break;
        }
        /* If pending mask not in sigmask, need set sigflag. */
        OsSigMaskSwitch(spcb, spcb->sig.sigprocmask);
    }
    SCHEDULER_UNLOCK(intSave);

    if (oldset != NULL) {
        retVal = LOS_ArchCopyToUser(&(oldset->sig[0]), &oldSigprocmask, sizeof(sigset_t));
        if (retVal != 0) {
            return -EFAULT;
        }
    }
    return ret;
}
//让进程的每一个task执行参数函数
int OsSigProcessForeachChild(LosProcessCB *spcb, ForEachTaskCB handler, void *arg)
{
    int ret;

    /* Visit the main thread last (if present) */	//最后访问主线程（如果有）
    LosTaskCB *taskCB = NULL;//遍历进程的 threadList 链表,里面存放的都是task节点
    LOS_DL_LIST_FOR_EACH_ENTRY(taskCB, &(spcb->threadSiblingList), LosTaskCB, threadList) {//遍历进程的任务列表
        ret = handler(taskCB, arg);//回调参数函数
        OS_RETURN_IF(ret != 0, ret);//这个宏的意思就是只有ret = 0时,啥也不处理.其余就返回 ret
    }
    return LOS_OK;
}
//信号处理函数,这里就是上面的 handler =  SigProcessSignalHandler,见于 OsSigProcessSend
static int SigProcessSignalHandler(LosTaskCB *tcb, void *arg)
{
    struct ProcessSignalInfo *info = (struct ProcessSignalInfo *)arg;//先把参数解出来
    int ret;
    int isMember;

    if (tcb == NULL) {
        return 0;
    }

    /* If the default tcb is not setted, then set this one as default. */
    if (!info->defaultTcb) {//如果没有默认发送方的TCB,就给一个.
        info->defaultTcb = tcb;
    }

    isMember = OsSigIsMember(&tcb->sig.sigwaitmask, info->sigInfo->si_signo);
    if (isMember && (!info->awakenedTcb)) {//这意味着任务正在等待此信号。
        /* This means the task is waiting for this signal. Stop looking for it and use this tcb.
         * The requirement is: if more than one task in this task group is waiting for the signal,
         * then only one indeterminate task in the group will receive the signal.
         */
        ret = OsTcbDispatch(tcb, info->sigInfo);//发送信号
        OS_RETURN_IF(ret < 0, ret);//这种写法很有意思

        /* set this tcb as awakenedTcb */
        info->awakenedTcb = tcb;
        OS_RETURN_IF(info->receivedTcb != NULL, SIG_STOP_VISIT); /* Stop search */
    }
    /* Is this signal unblocked on this thread? */
    isMember = OsSigIsMember(&tcb->sig.sigprocmask, info->sigInfo->si_signo);
    if ((!isMember) && (!info->receivedTcb) && (tcb != info->awakenedTcb)) {
        /* if unblockedTcb of this signal is not setted, then set it. */
        if (!info->unblockedTcb) {
            info->unblockedTcb = tcb;
        }

        ret = OsTcbDispatch(tcb, info->sigInfo);
        OS_RETURN_IF(ret < 0, ret);
        /* set this tcb as receivedTcb */
        info->receivedTcb = tcb;
        OS_RETURN_IF(info->awakenedTcb != NULL, SIG_STOP_VISIT); /* Stop search */
    }
    return 0; /* Keep searching */
}
//SIGKILL 干掉进程引发处理函数
static int SigProcessKillSigHandler(LosTaskCB *tcb, void *arg)
{
    struct ProcessSignalInfo *info = (struct ProcessSignalInfo *)arg;//转参

    if ((tcb != NULL) && (info != NULL) && (info->sigInfo != NULL)) {//进程有信号
        sig_cb *sigcb = &tcb->sig;
        if (!LOS_ListEmpty(&sigcb->waitList) && OsSigIsMember(&sigcb->sigwaitmask, info->sigInfo->si_signo)) {//waitlist 不为空 且 有信号
            OsTaskWake(tcb);//唤醒这个任务，加入进程的就绪队列
            OsSigEmptySet(&sigcb->sigwaitmask);//清空信号等待位,啥意思?
        }
    }
    return 0;
}
//进程加载task控制块信号
static void SigProcessLoadTcb(struct ProcessSignalInfo *info, siginfo_t *sigInfo)
{
    LosTaskCB *tcb = NULL;

    if (info->awakenedTcb == NULL && info->receivedTcb == NULL) {//信号即没有指定接收task 也没有指定被唤醒task
        if (info->unblockedTcb) {//如果进程信号信息体中有阻塞task
            tcb = info->unblockedTcb;//
        } else if (info->defaultTcb) {//如果有默认的发送方task
            tcb = info->defaultTcb;
        } else {
            return;
        }
        /* Deliver the signal to the selected task */
        (void)OsTcbDispatch(tcb, sigInfo);//向所选任务发送信号
    }
}
//给参数进程发送参数信号
int OsSigProcessSend(LosProcessCB *spcb, siginfo_t *sigInfo)
{
    int ret;
    struct ProcessSignalInfo info = {
        .sigInfo = sigInfo, //信号内容
        .defaultTcb = NULL,
        .unblockedTcb = NULL,
        .awakenedTcb = NULL,
        .receivedTcb = NULL
    };

    /* visit all taskcb and dispatch signal */ //访问所有任务和分发信号
    if ((info.sigInfo != NULL) && (info.sigInfo->si_signo == SIGKILL)) {//需要干掉进程时 SIGKILL = 9， #linux kill 9 14
        (void)OsSigProcessForeachChild(spcb, SigProcessKillSigHandler, &info);//进程要被干掉了,信息要通知其所有task知道
        OsSigAddSet(&spcb->sigShare, info.sigInfo->si_signo);
        OsWaitSignalToWakeProcess(spcb);//等待信号唤醒进程
        return 0;
    } else {
        ret = OsSigProcessForeachChild(spcb, SigProcessSignalHandler, &info);//给进程所有task发送信号
    }
    if (ret < 0) {
        return ret;
    }
    SigProcessLoadTcb(&info, sigInfo);
    return 0;
}
//清空信号
int OsSigEmptySet(sigset_t *set)
{
    *set = NULL_SIGNAL_SET;
    return 0;
}

/* Privilege process can't send to kernel and privilege process */ //特权进程无法发送到内核和特权进程
static int OsSignalPermissionToCheck(const LosProcessCB *spcb)
{
    UINT32 gid = spcb->group->groupID;

    if (gid == OS_KERNEL_PROCESS_GROUP) {//内核进程组
        return -EPERM;
    } else if (gid == OS_USER_PRIVILEGE_PROCESS_GROUP) {//用户特权进程组
        return -EPERM;
    }

    return 0;
}
//发送信号
int OsDispatch(pid_t pid, siginfo_t *info, int permission)
{
    LosProcessCB *spcb = OS_PCB_FROM_PID(pid);//找到这个进程
    if (OsProcessIsUnused(spcb)) {//进程是否还在使用
        return -ESRCH;
    }
#ifdef LOSCFG_SECURITY_CAPABILITY	//启用能力安全模式
    LosProcessCB *current = OsCurrProcessGet();//获取当前进程

    /* If the process you want to kill had been inactive, but still exist. should return LOS_OK */
    if (OsProcessIsInactive(spcb)) {//如果要终止的进程处于非活动状态，但仍然存在，应该返回OK
        return LOS_OK;
    }

    /* Kernel process always has kill permission and user process should check permission *///内核进程总是有kill权限，用户进程需要检查权限
    if (OsProcessIsUserMode(current) && !(current->processStatus & OS_PROCESS_FLAG_EXIT)) {//用户进程检查能力范围
        if ((current != spcb) && (!IsCapPermit(CAP_KILL)) && (current->user->userID != spcb->user->userID)) {
            return -EPERM;
        }
    }
#endif
    if ((permission == OS_USER_KILL_PERMISSION) && (OsSignalPermissionToCheck(spcb) < 0)) {//
        return -EPERM;
    }
    return OsSigProcessSend(spcb, info);//给参数进程发送信号
}
/************************************************
shell命令 kill 14 7（kill -14 7效果相同）
发送信号14（SIGALRM默认行为为进程终止）给7号进程
************************************************/
int OsKill(pid_t pid, int sig, int permission)
{
    siginfo_t info;
    int ret;

    /* Make sure that the para is valid */
    if (!GOOD_SIGNO(sig) || pid < 0) {//有效信号 [0,64]
        return -EINVAL;
    }
    if (OsProcessIDUserCheckInvalid(pid)) {//检查参数进程 
        return -ESRCH;
    }

    /* Create the siginfo structure */ //创建信号结构体
    info.si_signo = sig;	//信号编号
    info.si_code = SI_USER;	//来自用户进程信号
    info.si_value.sival_ptr = NULL;

    /* Send the signal */
    ret = OsDispatch(pid, &info, permission);//发送信号
    return ret;
}
//给发送杀死进程信号的过程加锁
int OsKillLock(pid_t pid, int sig)
{
    int ret;
    unsigned int intSave;

    SCHEDULER_LOCK(intSave);
    ret = OsKill(pid, sig, OS_USER_KILL_PERMISSION);//用户进程申请干掉其他进程,能成功吗? 满足条件就行
    SCHEDULER_UNLOCK(intSave);
    return ret;
}
//发送信号干掉线程
int OsPthreadKill(UINT32 tid, int signo)
{
    LosTaskCB *stcb = NULL;
    siginfo_t info;

    int ret;
    UINT32 intSave;

    /* Make sure that the signal is valid */
    OS_RETURN_IF(!GOOD_SIGNO(signo), -EINVAL);
    if (OS_TID_CHECK_INVALID(tid)) {
        return -ESRCH;
    }

    /* Create the siginfo structure */
    info.si_signo = signo;	//信号编号 如 SIGKILL
    info.si_code = SI_USER;	//来自用户进程信号
    info.si_value.sival_ptr = NULL;
    /* Keep things stationary through the following */
    SCHEDULER_LOCK(intSave);
    /* Get the TCB associated with the thread */
    stcb = OsGetTaskCB(tid);
    OS_GOTO_EXIT_IF(stcb == NULL, -ESRCH);

    ret = OsUserTaskOperatePermissionsCheck(stcb);//用户态操作权限检查
    OS_GOTO_EXIT_IF(ret != LOS_OK, -ret);

    /* Dispatch the signal to thread, bypassing normal task group thread
     * dispatch rules. */
    ret = OsTcbDispatch(stcb, &info);//将信号发送到线程，将绕过正常的任务组线程调度规则
EXIT:
    SCHEDULER_UNLOCK(intSave);
    return ret;
}
//添加信号到指定信号集
int OsSigAddSet(sigset_t *set, int signo)
{
    /* Verify the signal */
    if (!GOOD_SIGNO(signo)) {
        return -EINVAL;
    } else {
        /* In musl, sig No bits 00000100 present sig No 3, but  1<< 3 = 00001000, so signo needs minus 1 */
        signo -= 1;
        /* Add the signal to the set */
        *set |= SIGNO2SET((unsigned int)signo);//填充信号集
        return LOS_OK;
    }
}
//获取当前进程的阻塞信号集
int OsSigPending(sigset_t *set)
{
    LosTaskCB *tcb = NULL;
    unsigned int intSave;

    if (set == NULL) {
        return -EFAULT;
    }

    SCHEDULER_LOCK(intSave);
    tcb = OsCurrTaskGet();
    *set = tcb->sig.sigPendFlag;//因何而被阻塞
    SCHEDULER_UNLOCK(intSave);
    return LOS_OK;
}

STATIC int FindFirstSetedBit(UINT64 n)
{
    int count;

    if (n == 0) {
        return -1;
    }
    for (count = 0; (count < UINT64_BIT_SIZE) && (n ^ 1ULL); n >>= 1, count++) {}
    return (count < UINT64_BIT_SIZE) ? count : (-1);
}

int OsSigTimedWaitNoLock(sigset_t *set, siginfo_t *info, unsigned int timeout)
{
    LosTaskCB *task = NULL;
    sig_cb *sigcb = NULL;
    int ret;

    task = OsCurrTaskGet();
    sigcb = &task->sig;

    if (sigcb->waitList.pstNext == NULL) {
        LOS_ListInit(&sigcb->waitList);//初始化信号等待链表
    }
    /* If pendingflag & set > 0, shound clear pending flag */
    sigset_t clear = sigcb->sigPendFlag & *set;
    if (clear) {
        sigcb->sigPendFlag ^= clear;
        ret = FindFirstSetedBit((UINT64)clear) + 1;
    } else {
        OsSigAddSet(set, SIGKILL);//kill 9 14 必须要处理
        OsSigAddSet(set, SIGSTOP);//终止进程的信号也必须处理

        sigcb->sigwaitmask |= *set;//按位加到等待集上,也就是说sigwaitmask的信号来了都是要处理的.
        ret = OsTaskWait(&sigcb->waitList, timeout, TRUE);//将当前任务挂到waitlist上
        if (ret == LOS_ERRNO_TSK_TIMEOUT) {
            ret = -EAGAIN;
        }
        sigcb->sigwaitmask = NULL_SIGNAL_SET;
    }
    if (info != NULL) {
        (void) memcpy_s(info, sizeof(siginfo_t), &sigcb->sigunbinfo, sizeof(siginfo_t));
    }
    return ret;
}
//让当前任务等待的信号
int OsSigTimedWait(sigset_t *set, siginfo_t *info, unsigned int timeout)
{
    int ret;
    unsigned int intSave;

    SCHEDULER_LOCK(intSave);

    ret = OsSigTimedWaitNoLock(set, info, timeout);//以不加锁的方式等待

    SCHEDULER_UNLOCK(intSave);
    return ret;
}
//让当前任务暂停的信号
int OsPause(void)
{
    LosTaskCB *spcb = NULL;
    sigset_t oldSigprocmask;

    spcb = OsCurrTaskGet();
    oldSigprocmask = spcb->sig.sigprocmask;
    return OsSigSuspend(&oldSigprocmask);
}
//让当前任务暂停的信号
int OsSigSuspend(const sigset_t *set)
{
    unsigned int intSave;
    LosTaskCB *rtcb = NULL;
    unsigned int sigTempProcMask;
    sigset_t setSuspend;
    int ret;

    if (set == NULL) {
        return -EINVAL;
    }
    SCHEDULER_LOCK(intSave);
    rtcb = OsCurrTaskGet();
    sigTempProcMask = rtcb->sig.sigprocmask;

    /* Wait signal calc */
    setSuspend = FULL_SIGNAL_SET & (~(*set));

    /* If pending mask not in sigmask, need set sigflag */
    OsSigMaskSwitch(rtcb, *set);
    ret = OsSigTimedWaitNoLock(&setSuspend, NULL, LOS_WAIT_FOREVER);
    if (ret < 0) {
        PRINT_ERR("FUNC %s LINE = %d, ret = %x\n", __FUNCTION__, __LINE__, ret);
    }
    /* Restore old sigprocmask */
    OsSigMaskSwitch(rtcb, sigTempProcMask);

    SCHEDULER_UNLOCK(intSave);
    return -EINTR;
}
/**************************************************
安装信号,函数用于改变进程接收到特定信号后的行为。
sig:信号的值，可以为除SIGKILL及SIGSTOP外的任何一个特定有效的信号（为这两个信号定义自己的处理函数，将导致信号安装错误）。
act:指向结构sigaction的一个实例的指针，在结构sigaction的实例中，指定了对特定信号的处理，可以为空，进程会以缺省方式对信号处理；
oldact:指向的对象用来保存返回的原来对相应信号的处理，可指定oldact为NULL。
如果把第二、第三个参数都设为NULL，那么该函数可用于检查信号的有效性。
**************************************************/
int OsSigAction(int sig, const sigaction_t *act, sigaction_t *oact)
{
    UINTPTR addr;
    sigaction_t action;

    if (!GOOD_SIGNO(sig) || sig < 1 || act == NULL) {
        return -EINVAL;
    }
    if (LOS_ArchCopyFromUser(&action, act, sizeof(sigaction_t)) != LOS_OK) {
        return -EFAULT;
    }

    if (sig == SIGSYS) {//系统调用中参数错，如系统调用号非法 
        addr = OsGetSigHandler();//获取进程信号处理函数
        if (addr == 0) {
            OsSetSigHandler((unsigned long)(UINTPTR)action.sa_handler);//设置进程信号处理函数
            return LOS_OK;
        }
        return -EINVAL;
    }

    return LOS_OK;
}
/**********************************************
产生系统调用时,也就是软中断时,保存用户栈寄存器现场信息
**********************************************/
void OsSaveSignalContext(unsigned int *sp)
{
    UINTPTR sigHandler;
    UINT32 intSave;
    LosTaskCB *task = NULL;
    LosProcessCB *process = NULL;
    sig_cb *sigcb = NULL;
    unsigned long cpsr;

    OS_RETURN_IF_VOID(sp == NULL);
    cpsr = OS_SYSCALL_GET_CPSR(sp);

    OS_RETURN_IF_VOID(((cpsr & CPSR_MASK_MODE) != CPSR_USER_MODE));//必须在用户模式下保存
    SCHEDULER_LOCK(intSave);
    task = OsCurrTaskGet();
    process = OsCurrProcessGet();
    sigcb = &task->sig;//获取任务的信号控制块

    if ((sigcb->context.count == 0) && ((sigcb->sigFlag != 0) || (process->sigShare != 0))) {//未保存上下文且关注了信号
        sigHandler = OsGetSigHandler();
        if (sigHandler == 0) {//信号没有注册
            sigcb->sigFlag = 0;
            process->sigShare = 0;
            SCHEDULER_UNLOCK(intSave);
            PRINT_ERR("The signal processing function for the current process pid =%d is NULL!\n", task->processID);
            return;
        }
        /* One pthread do the share signal */ 
        sigcb->sigFlag |= process->sigShare;//记录由一个线程执行可进程的共享信号,这些恢复上下文时就找到对应的任务
        unsigned int signo = (unsigned int)FindFirstSetedBit(sigcb->sigFlag) + 1;
        OsProcessExitCodeSignalSet(process, signo);
        sigcb->context.CPSR = cpsr;		//保存当前各寄存器的信息
        sigcb->context.PC = sp[REG_PC];
        sigcb->context.USP = sp[REG_SP];
        sigcb->context.ULR = sp[REG_LR];
        sigcb->context.R0 = sp[REG_R0];
        sigcb->context.R1 = sp[REG_R1];
        sigcb->context.R2 = sp[REG_R2];
        sigcb->context.R3 = sp[REG_R3];
        sigcb->context.R7 = sp[REG_R7];
        sigcb->context.R12 = sp[REG_R12];
        sp[REG_PC] = sigHandler;//下一个要执行的函数,信号注册函数
        sp[REG_R0] = signo;		//信号注册函数参数
        sp[REG_R1] = (unsigned int)(UINTPTR)(sigcb->sigunbinfo.si_value.sival_ptr); //@note_why 这里没看明白是什么意思?
        /* sig No bits 00000100 present sig No 3, but  1<< 3 = 00001000, so signo needs minus 1 */
        sigcb->sigFlag ^= 1ULL << (signo - 1);
        sigcb->context.count++;	//代表已保存
    }

    SCHEDULER_UNLOCK(intSave);
}
//发生硬中断时,需保存用户态的用户栈现场,多了一个参数 R7寄存器
//汇编调用 见于 los_dispatch.S | 254行:    BL      OsSaveSignalContextIrq
void OsSaveSignalContextIrq(unsigned int *sp, unsigned int r7)
{
    UINTPTR sigHandler;
    LosTaskCB *task = NULL;
    LosProcessCB *process = NULL;
    sig_cb *sigcb = NULL;
    unsigned long cpsr;
    UINT32 intSave;
    TaskIrqContext *context = (TaskIrqContext *)(sp);

    OS_RETURN_IF_VOID(sp == NULL);
    cpsr = context->CPSR;
    OS_RETURN_IF_VOID(((cpsr & CPSR_MASK_MODE) != CPSR_USER_MODE));//必须在用户模式下保存用户栈信息

    SCHEDULER_LOCK(intSave);
    task = OsCurrTaskGet();
    process = OsCurrProcessGet();
    sigcb = &task->sig;
    if ((sigcb->context.count == 0) && ((sigcb->sigFlag != 0) || (process->sigShare != 0))) {
        sigHandler = OsGetSigHandler();
        if (sigHandler == 0) {
            sigcb->sigFlag = 0;
            process->sigShare = 0;
            SCHEDULER_UNLOCK(intSave);
            PRINT_ERR("The current process pid =%d starts fail!\n", task->processID);
            return;
        }
        sigcb->sigFlag |= process->sigShare;
        unsigned int signo = (unsigned int)FindFirstSetedBit(sigcb->sigFlag) + 1;
        OsProcessExitCodeSignalSet(process, signo);
        (VOID)memcpy_s(&sigcb->context.R0, sizeof(TaskIrqDataSize), &context->R0, sizeof(TaskIrqDataSize));//note_why 为何此处和OsSaveSignalContext的处理不一致?
        sigcb->context.R7 = r7;
        context->PC = sigHandler;
        context->R0 = signo;
        context->R1 = (UINT32)(UINTPTR)sigcb->sigunbinfo.si_value.sival_ptr;
        /* sig No bits 00000100 present sig No 3, but  1<< 3 = 00001000, so signo needs minus 1 */
        sigcb->sigFlag ^= 1ULL << (signo - 1);
        sigcb->context.count++;
    }
    SCHEDULER_UNLOCK(intSave);
}
/****************************************************
恢复信号上下文,由系统调用之__NR_sigreturn产生,这是一个内部产生的系统调用.
为什么要恢复呢?
因为系统调用的执行由任务内核态完成,使用的栈也是内核栈,CPU相关寄存器记录的都是内核栈的内容,
而系统调用完成后,需返回任务的用户栈执行,这时需将CPU各寄存器回到用户态现场
所以函数的功能就变成了还原寄存器的值
****************************************************/
void OsRestorSignalContext(unsigned int *sp)
{
    LosTaskCB *task = NULL; /* Do not adjust this statement */
    LosProcessCB *process = NULL;
    sig_cb *sigcb = NULL;
    UINT32 intSave;

    SCHEDULER_LOCK(intSave);
    task = OsCurrTaskGet();
    sigcb = &task->sig;//获取当前任务信号控制块

    if (sigcb->context.count != 1) {//必须之前保存过,才能被恢复
        SCHEDULER_UNLOCK(intSave);
        PRINT_ERR("sig error count : %d\n", sigcb->context.count);
        return;
    }

    process = OsCurrProcessGet();//获取当前进程
    sp[REG_PC] = sigcb->context.PC;//指令寄存器
    OS_SYSCALL_SET_CPSR(sp, sigcb->context.CPSR);//重置程序状态寄存器
    sp[REG_SP] = sigcb->context.USP;//堆栈指针, USP指的是 用户态的堆栈,即将回到用户栈继续运行
    sp[REG_LR] = sigcb->context.ULR;//连接寄存器
    sp[REG_R0] = sigcb->context.R0;
    sp[REG_R1] = sigcb->context.R1;
    sp[REG_R2] = sigcb->context.R2;
    sp[REG_R3] = sigcb->context.R3;
    sp[REG_R7] = sigcb->context.R7;
    sp[REG_R12] = sigcb->context.R12;
    sigcb->context.count--;	//信号上下文的数量回到减少
    process->sigShare = 0;	//回到用户态,信号共享清0
    OsProcessExitCodeSignalClear(process);//清空进程退出码
    SCHEDULER_UNLOCK(intSave);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
