[鸿蒙内核源码注释中文版 【 Gitee仓 ](https://gitee.com/weharmony/kernel_liteos_a_note) | [ CSDN仓 ](https://codechina.csdn.net/kuangyufei/kernel_liteos_a_note) | [ Github仓 ](https://github.com/kuangyufei/kernel_liteos_a_note) | [ Coding仓 】](https://weharmony.coding.net/public/harmony/kernel_liteos_a_note/git/files) 项目中文注解鸿蒙官方内核源码,图文并茂,详细阐述鸿蒙架构和代码设计细节.每个码农,学职生涯,都应精读一遍内核源码.精读内核源码最大的好处是:将孤立知识点织成一张高浓度,高密度底层网,对计算机底层体系化理解形成永久记忆,从此高屋建瓴分析/解决问题.

[鸿蒙源码分析系列篇 【 CSDN ](https://blog.csdn.net/kuangyufei/article/details/108727970) [| OSCHINA ](https://my.oschina.net/u/3751245/blog/4626852) [| WIKI 】](https://gitee.com/weharmony/kernel_liteos_a_note/wikis/pages) 从 HarmonyOS 架构层视角整理成文, 并首创用生活场景讲故事的方式试图去解构内核，一窥究竟。

---

## **[kernel\_liteos\_a_note](https://gitee.com/weharmony/kernel_liteos_a_note): 鸿蒙内核源码注释中文版 -> 点击目录和文件查看源码的详细中文注解**  

可以肯定是以下问题在一行行的源码中都能找到答案

---

- [kernel_liteos_a_note](https://gitee.com/weharmony/kernel_liteos_a_note/)
  * [kernel](https://gitee.com/weharmony/kernel_liteos_a_note/tree/master/kernel/)
    + [base](https://gitee.com/weharmony/kernel_liteos_a_note/tree/master/kernel/base/)
    	+ [core](https://gitee.com/weharmony/kernel_liteos_a_note/tree/master/kernel/base/core/)
    		+ [los_bitmap.c](https://gitee.com/weharmony/kernel_liteos_a_note/tree/master/kernel/base/core/los_bitmap.c) -> []() -> 位图管理器有什么作用 ? 在内核常应用于哪些场景 ?
    		+ [los_process.c](https://gitee.com/weharmony/kernel_liteos_a_note/tree/master/kernel/base/core/los_process.c) -> [鸿蒙内核源码分析(进程管理篇)](https://blog.csdn.net/kuangyufei/article/details/108595941) -> 进程是内核的资源管理单元,它是如何管理 任务, 内存,文件的 ? 进程间是如何协作的 ?
    		+ [los_sortlink.c](https://gitee.com/weharmony/kernel_liteos_a_note/tree/master/kernel/base/core/los_sortlink.c) -> []() -> 排序链表的实现,它的应用场景是怎样的 ? 
    		+ [los_swtmr.c](https://gitee.com/weharmony/kernel_liteos_a_note/tree/master/kernel/base/core/los_swtmr.c) -> []() -> 内核的定时器是如何实现和管理的 ?
    		+ [los_sys.c](https://gitee.com/weharmony/kernel_liteos_a_note/tree/master/kernel/base/core/los_sys.c) -> []() -> 几个跟tick相关的转化函数
    		+ [los_task.c](https://gitee.com/weharmony/kernel_liteos_a_note/tree/master/kernel/base/core/los_task.c) -> [鸿蒙内核源码分析(Task管理篇)](https://blog.csdn.net/kuangyufei/article/details/108661248) -> Task是内核调度的单元,它解决了什么问题 ? 如何调度 ?
    		+ [los_tick.c](https://gitee.com/weharmony/kernel_liteos_a_note/tree/master/kernel/base/core/los_tick.c) -> [鸿蒙内核源码分析(时钟管理篇)](https://blog.csdn.net/kuangyufei/article/details/108603468) ->  是谁在一直触发调度 ? 硬时钟中断都干了些什么事?
    		+ [los_timeslice.c](https://gitee.com/weharmony/kernel_liteos_a_note/tree/master/kernel/base/core/los_timeslice.c) -> []() -> 进程和任务能一直占有CPU吗 ? 怎么合理的分配时间 ?
    	+ [ipc](https://gitee.com/weharmony/kernel_liteos_a_note/tree/master/kernel/base/ipc/) -> []() -> 进程间通讯有哪些方式 ? 请说出三种 ? 是如何实现的 ?
    		+ [los_event.c](https://gitee.com/weharmony/kernel_liteos_a_note/tree/master/kernel/ipc/base/los_event.c) -> []() -> 事件解决了什么问题 ? 怎么管理的 ?
    		+ [los_futex.c](https://gitee.com/weharmony/kernel_liteos_a_note/tree/master/kernel/ipc/base/los_futex.c) -> []() -> futex 是Fast Userspace muTexes的缩写(快速用户空间互斥体),它有什么作用 ?
    		+ [los_ipcdebug.c](https://gitee.com/weharmony/kernel_liteos_a_note/tree/master/kernel/ipc/base/los_ipcdebug.c) -> []() -> 进程间通讯如何调试 ?
    		+ [los_mux.c](https://gitee.com/weharmony/kernel_liteos_a_note/tree/master/kernel/ipc/base/los_mux.c) -> []() -> 互斥量,有你没我的零和博弈, 为什么需要互斥量 ? 是如何实现的 ?
    		+ [los_queue.c](https://gitee.com/weharmony/kernel_liteos_a_note/tree/master/kernel/ipc/base/los_queue.c) -> []() -> 内核消息队列是如何实现的 ? 对长度和大小有限制吗 ? 
    		+ [los_queue_debug.c](https://gitee.com/weharmony/kernel_liteos_a_note/tree/master/kernel/ipc/base/los_queue_debug.c) -> []() ->  如何调试消息队列 ? 
    		+ [los_sem.c](https://gitee.com/weharmony/kernel_liteos_a_note/tree/master/kernel/ipc/base/los_sem.c) -> []() -> 信号量解决了什么问题 ? 它的本质是什么 ?
    		+ [los_sem_debug.c](https://gitee.com/weharmony/kernel_liteos_a_note/tree/master/kernel/ipc/base/los_sem_debug.c) -> []() -> 如何调试信号量 ?
    		+ [los_signal.c](https://gitee.com/weharmony/kernel_liteos_a_note/tree/master/kernel/ipc/base/los_signal.c) -> []() -> 信号解决了什么问题? 你知道哪些信号 ?
        + [mem](https://gitee.com/weharmony/kernel_liteos_a_note/tree/master/kernel/base/mem/) -> []() -> 
    	+ [misc](https://gitee.com/weharmony/kernel_liteos_a_note/tree/master/kernel/base/misc/) -> []() -> 
    		+ [kill_shellcmd.c](https://gitee.com/weharmony/kernel_liteos_a_note/tree/master/kernel/base/misc/kill_shellcmd.c) -> []() -> shell命令kill的实现,熟悉的 kill 9 18 的背后发生了什么?
            + [los_misc.c](https://gitee.com/weharmony/kernel_liteos_a_note/tree/master/kernel/base/misc/los_misc.c) -> []() -> 
            + [los_stackinfo.c](https://gitee.com/weharmony/kernel_liteos_a_note/tree/master/kernel/base/misc/los_stackinfo.c) -> []() -> 栈有哪些信息 ? 如何检测栈是否异常 ?
            + [mempt_shellcmd.c](https://gitee.com/weharmony/kernel_liteos_a_note/tree/master/kernel/base/misc/mempt_shellcmd.c) -> []() -> 和内存相关的shell命令有哪些 ? 
            + [swtmr_shellcmd.c](https://gitee.com/weharmony/kernel_liteos_a_note/tree/master/kernel/base/misc/swtmr_shellcmd.c) -> []() -> 和软时钟相关的shell命令有哪些 ? 
            + [sysinfo_shellcmd.c](https://gitee.com/weharmony/kernel_liteos_a_note/tree/master/kernel/base/misc/sysinfo_shellcmd.c) -> []() -> 和系统信息相关的shell命令有哪些 ? 
            + [task_shellcmd.c](https://gitee.com/weharmony/kernel_liteos_a_note/tree/master/kernel/base/misc/task_shellcmd.c) -> []() -> 和任务相关的shell命令有哪些 ? 
            + [vm_shellcmd.c](https://gitee.com/weharmony/kernel_liteos_a_note/tree/master/kernel/base/misc/vm_shellcmd.c) -> []() -> 和虚拟内存相关的shell命令有哪些 ? 
        + [mp](https://gitee.com/weharmony/kernel_liteos_a_note/tree/master/kernel/base/mp/) -> []() ->
            + [los_lockdep.c](https://gitee.com/weharmony/kernel_liteos_a_note/tree/master/kernel/base/mp/los_lockdep.c) -> []() -> 死锁是怎么发生的 ? 如何检测死锁 ?
            + [los_mp.c](https://gitee.com/weharmony/kernel_liteos_a_note/tree/master/kernel/base/mp/los_mp.c) -> []() -> 鸿蒙支持多CPU吗 ? 它们是如何工作的? CPU之间是如何通讯的 ?
            + [los_percpu.c](https://gitee.com/weharmony/kernel_liteos_a_note/tree/master/kernel/base/mp/los_percpu.c) -> []() ->  CPU有哪些信息 ?
            + [los_stat.c](https://gitee.com/weharmony/kernel_liteos_a_note/tree/master/kernel/base/mp/los_stat.c) -> []() -> CPU的运行信息如何统计 ?
        + [om](https://gitee.com/weharmony/kernel_liteos_a_note/tree/master/kernel/base/om/) -> []() ->
            + [los_err.c](https://gitee.com/weharmony/kernel_liteos_a_note/tree/master/kernel/base/om/los_err.c) -> []() ->
        + [sched/sched_sq](https://gitee.com/weharmony/kernel_liteos_a_note/tree/master/kernel/base/sched/sched_sq/) -> []() ->
            + [los_priqueue.c](https://gitee.com/weharmony/kernel_liteos_a_note/tree/master/kernel/base/sched/sched_sq/los_priqueue.c) -> [鸿蒙内核源码分析(调度队列篇)](https://blog.csdn.net/kuangyufei/article/details/108626671) -> 为什么只有就绪状态才会有队列 ? 
            + [los_sched.c](https://gitee.com/weharmony/kernel_liteos_a_note/tree/master/kernel/base/sched/sched_sq/los_sched.c) -> [鸿蒙内核源码分析(调度机制篇)](https://blog.csdn.net/kuangyufei/article/details/108705968) -> 哪些情况下会触发调度 ? 调度算法是怎样的 ?
        + [vm](https://gitee.com/weharmony/kernel_liteos_a_note/tree/master/kernel/base/vm/) -> [鸿蒙内核源码分析(内存规则篇)](https://blog.csdn.net/kuangyufei/article/details/109437223) -> 什么是虚拟内存 ? 虚拟内存全景图是怎样的 ?
            + [los_vm_boot.c](https://gitee.com/weharmony/kernel_liteos_a_note/tree/master/kernel/base/vm/los_vm_boot.c) -> []() -> 开机阶段内存是如何初始化的 ?
            + [los_vm_dump.c](https://gitee.com/weharmony/kernel_liteos_a_note/tree/master/kernel/base/vm/los_vm_dump.c) -> []() -> 如何 dump 内存数据 ?
            + [los_vm_fault.c](https://gitee.com/weharmony/kernel_liteos_a_note/tree/master/kernel/base/vm/los_vm_fault.c) -> []() -> 为什么会缺页 ? 缺页怎么处理 ?
            + [los_vm_filemap.c](https://gitee.com/weharmony/kernel_liteos_a_note/tree/master/kernel/base/vm/los_vm_filemap.c) -> []() -> 文件和内存是如何映射? 什么是 写时拷贝技术(cow) ?
            + [los_vm_iomap.c](https://gitee.com/weharmony/kernel_liteos_a_note/tree/master/kernel/base/vm/los_vm_iomap.c) -> []() -> 设备和内存是如何映射 ? 
            + [los_vm_map.c](https://gitee.com/weharmony/kernel_liteos_a_note/tree/master/kernel/base/vm/los_vm_map.c) -> [鸿蒙内核源码分析(内存映射篇)](https://blog.csdn.net/kuangyufei/article/details/109032636) -> 内核空间,用户空间,线性区是如何分配的,虚拟内存<-->物理内存是如何映射的 ?
            + [los_vm_page.c](https://gitee.com/weharmony/kernel_liteos_a_note/tree/master/kernel/base/vm/los_vm_page.c) -> []() -> 什么是物理页框,哪些地方会用到它 ? 
            + [los_vm_phys.c](https://gitee.com/weharmony/kernel_liteos_a_note/tree/master/kernel/base/vm/los_vm_phys.c) -> [鸿蒙内核源码分析(物理内存篇)](https://blog.csdn.net/kuangyufei/article/details/111765600) -> 段页式管理,物理内存是如何分配和回收的 ?
            + [los_vm_scan.c](https://gitee.com/weharmony/kernel_liteos_a_note/tree/master/kernel/base/vm/los_vm_scan.c) -> []() -> LRU算法是如何运作的 ?
            + [los_vm_syscall.c](https://gitee.com/weharmony/kernel_liteos_a_note/tree/master/kernel/base/vm/los_vm_syscall.c) -> []() -> 系统调用之内存, 用户进程如何申请内存 ? 底层发生了什么 ?
            + [oom.c](https://gitee.com/weharmony/kernel_liteos_a_note/tree/master/kernel/base/vm/oom.c) -> []() -> 内存溢出是如何检测的 ?
            + [shm.c](https://gitee.com/weharmony/kernel_liteos_a_note/tree/master/kernel/base/vm/shm.c) -> []() -> 共享内存是如何实现的 ?
    + [common](https://gitee.com/weharmony/kernel_liteos_a_note/tree/master/kernel/common/) -> []() ->
        + [console.c](https://gitee.com/weharmony/kernel_liteos_a_note/tree/master/kernel/common/console.c) -> []() -> 熟悉的控制台是如何实现的 ?
        + [hwi_shell.c](https://gitee.com/weharmony/kernel_liteos_a_note/tree/master/kernel/common/hwi_shell.c) -> []() -> 如何查询硬件中断 ?
        + [los_cir_buf.c](https://gitee.com/weharmony/kernel_liteos_a_note/tree/master/kernel/common/los_cir_buf.c) -> []() -> 环形缓冲区的读写是如何实现的 ? 常用于什么场景下 ?
        + [los_config.c](https://gitee.com/weharmony/kernel_liteos_a_note/tree/master/kernel/common/los_config.c) -> []() -> 内核有哪些配置信息 ?
        + [los_exc_interaction.c](https://gitee.com/weharmony/kernel_liteos_a_note/tree/master/kernel/common/los_exc_interaction.c) -> []() -> 任务出现异常如何检测 ?
        + [los_excinfo.c](https://gitee.com/weharmony/kernel_liteos_a_note/tree/master/kernel/common/los_excinfo.c) -> []() -> 异常有哪些信息 ? 如何记录异常信息 ?
        + [los_hilog.c](https://gitee.com/weharmony/kernel_liteos_a_note/tree/master/kernel/common/los_hilog.c) -> []() -> 内核是如何封装日志的 ?
        + [los_magickey.c](https://gitee.com/weharmony/kernel_liteos_a_note/tree/master/kernel/common/los_magickey.c) -> []() -> 魔法键有什么作用 ?
        + [los_printf.c](https://gitee.com/weharmony/kernel_liteos_a_note/tree/master/kernel/common/los_printf.c) -> []() -> 内核对 printf 做了哪些封装 ?
        + [los_rootfs.c](https://gitee.com/weharmony/kernel_liteos_a_note/tree/master/kernel/common/los_rootfs.c) -> []() -> 什么是根文件系统 ? 为什么需要它 ?
        + [los_seq_buf.c](https://gitee.com/weharmony/kernel_liteos_a_note/tree/master/kernel/common/los_seq_buf.c) -> []() ->
        + [virtual_serial.c](https://gitee.com/weharmony/kernel_liteos_a_note/tree/master/kernel/common/virtual_serial.c) -> []() -> 如何实现访问串口如同访问文件一样方便 ?
    + [extended](https://gitee.com/weharmony/kernel_liteos_a_note/kernel/extended/tree/master/) -> []() ->
        + [cppsupport](https://gitee.com/weharmony/kernel_liteos_a_note/kernel/extended/tree/master/cppsupport/) -> []() -> 
            + [los_cppsupport.c](https://gitee.com/weharmony/kernel_liteos_a_note/tree/master/kernel/extended/cppsupport/los_cppsupport.c) -> []() -> 对C++是如何支持的 ? 
        + [cpup](https://gitee.com/weharmony/kernel_liteos_a_note/tree/master/kernel/extended/cpup/) -> []() ->
            + [cpup_shellcmd.c](https://gitee.com/weharmony/kernel_liteos_a_note/tree/master/kernel/extended/cpup/cpup_shellcmd.c) -> []() -> 如何实时查询系统CPU的占用率 ? 
            + [los_cpup.c](https://gitee.com/weharmony/kernel_liteos_a_note/tree/master/kernel/extended/cpup/los_cpup.c) -> []() -> 内核如何做到实时统计CPU性能的 ?
        + [dynload/src](https://gitee.com/weharmony/kernel_liteos_a_note/tree/master/kernel/extended/dynload/src/) []() ->
            + [los_exec_elf.c](https://gitee.com/weharmony/kernel_liteos_a_note/tree/master/kernel/extended/dynload/src/los_exec_elf.c) -> []() -> 鸿蒙如何运行ELF ? 什么是腾笼换鸟技术 ?
            + [los_load_elf.c](https://gitee.com/weharmony/kernel_liteos_a_note/tree/master/kernel/extended/dynload/src/los_load_elf.c) -> []() -> 鸿蒙如何动态加载 ELF ?  
        + [liteipc](https://gitee.com/weharmony/kernel_liteos_a_note/tree/master/kernel/extended/liteipc/) -> []() ->
            + [hm_liteipc.c](https://gitee.com/weharmony/kernel_liteos_a_note/tree/master/kernel/extended/liteipc/hm_liteipc.c) -> []() -> 如何用文件的方式读取消息队列 ? liteipc和普通消息队列区别有哪些 ?
        + [tickless](https://gitee.com/weharmony/kernel_liteos_a_note/tree/master/kernel/extended/tickless/) -> []() ->
            + [los_tickless.c](https://gitee.com/weharmony/kernel_liteos_a_note/tree/master/kernel/extended/tickless/los_tickless.c) -> []() -> 新定时机制新在哪里 ? 它解决了哪些问题 ?
        + [trace](https://gitee.com/weharmony/kernel_liteos_a_note/tree/master/kernel/extended/trace/) -> []() ->
            + [los_trace.c](https://gitee.com/weharmony/kernel_liteos_a_notetree/master/kernel/extended/los_trace.c) -> []() -> 如何实现跟踪 ? 内核在跟踪什么 ?
        + [vdso](https://gitee.com/weharmony/kernel_liteos_a_note/tree/master/kernel/extended/vdso/) -> []() -> 用户空间访问内核空间有哪些途径 ?
            + [src](https://gitee.com/weharmony/kernel_liteos_a_note/tree/master/kernel/extended/vdso/src/) -> []() ->
                + [los_vdso.c](https://gitee.com/weharmony/kernel_liteos_a_note/tree/master/kernel/extended/vdso/src/los_vdso.c) -> []() -> VDSO(Virtual Dynamically-linked Shared Object) 是如何实现的 ?
                + [los_vdso_text.S](https://gitee.com/weharmony/kernel_liteos_a_note/tree/master/kernel/extended/vdso/src/los_vdso_text.S) -> []() -> 
            + [usr](https://gitee.com/weharmony/kernel_liteos_a_note/tree/master/kernel/extended/vdso/usr/) -> []() ->
                + [los_vdso_sys.c](https://gitee.com/weharmony/kernel_liteos_a_note/tree/master/kernel/extended/vdso/usr/los_vdso_sys.c) -> []() ->      
    + [user/src](https://gitee.com/weharmony/kernel_liteos_a_note/tree/master/kernel/user/src/) -> []() ->
    	+ [los_user_init.c](https://gitee.com/weharmony/kernel_liteos_a_note/tree/master/kernel/user/src/los_user_init.c) -> []() ->









---

系列篇文章 进入 >\> [鸿蒙系统源码分析(总目录) 【 CSDN](https://blog.csdn.net/kuangyufei/article/details/108727970) | [OSCHINA](https://my.oschina.net/u/3751245/blog/4626852) [| WIKI 】](https://gitee.com/weharmony/kernel_liteos_a_note/wikis/pages)查看
    
 注释中文版 进入 >\> [鸿蒙内核源码注释中文版 【 Gitee仓](https://gitee.com/weharmony/kernel_liteos_a_note) | [CSDN仓](https://codechina.csdn.net/kuangyufei/kernel_liteos_a_note) | [Github仓](https://github.com/kuangyufei/kernel_liteos_a_note) | [Coding仓 】](https://weharmony.coding.net/public/harmony/kernel_liteos_a_note/git/files)阅读
    