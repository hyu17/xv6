# Using GDB
这个lab设置在这里的主要目的是为了让我们没有了解过GDB的同学，能够快速上手GDB的一些常见命令，本身并没有什么难度。

# System call tracing
## 执行步骤
1. 阅读xv6 book，了解本堂课程的内容，对lab做的事情有所了解。
2. 阅读`trace.c`，了解用户视角中的trace是做什么工作：It should take one argument, an integer "mask", whose bits specify which system calls to trace. 再根据题目中给出的例子：`$ trace 32 grep hello README`，我们能够知道执行完`trace`之后，还会执行一些system calls，这个`mask`代表了接下来执行的所有system calls，因此后面在判断执行了哪些system call的时候，这个mask就比较关键了。
3. 修改`user/user.h`以及`user/usys.pl`，添加`trace`这个用户视角的system call的声明，使得用户能够使用到它以及存下trace的entry，以便执行`trace`的时候通过`ecall SYS_trace`在kernel中正确的调用对应的`sys_trace` function。
4. 在`kernel/sysproc.c`中实现`sys_trace()` function，参考其他函数读取用户传入的argument的方式，我认为这样做的原因是：kernel不应该相信user，所以对于user传入的所有参数，都应该检查无误之后再使用，这里会将从user mode中传入的`mask`读取到`struct proc`里的一个新的变量中（我声明的叫做`umask`），选择存取在`struct proc`里的原因是：每一次system call之后都会从kernel mode返回到user mode，但是都是当前这个process在做这件事情，并且有可能会trace多个system calls，因此存取在`struct proc`将在下一次陷入kernel的时候非常方便的进行判断。同时也需要在`kernel/syscall.h`中添加`SYS_trace`这个syscall number。（注释：其实内核的代码本质上也是程序员实现的，也是通过C一点一点实现出来的，因此上述这个过程，通过阅读源码的方式就能很清晰的知道为什么会做这些操作）
5. 由于我们更改了`struct proc`里的内容，所以理所应当在`fork`中也进行对应的修改，保证child process与parent process的内容是保持一致的。
6. 根据题意，`trace` system call跟踪的是`1 << SYS_fork`，并且前面有讲到有可能一次trace多个system calls，而每个system call只能有一个陷入内核，所以只能在每次陷入内核后在`syscall()`中判断该system call是否被trace了，判断的方法就是`(1 << SYS_number) & p->umask`是否不等于0.

至此，这个问题就解决了。
## 收获
解决了这个问题之后，对system call的执行流程有了一个宏观的理解，对professor在课堂上讲的system call的执行流程（即`trace()->ecall SYS_trace->sys_trace()`）了解的更深刻了。
## 遇到的困难
1. 做这个问题遇到的最大的困难是如何判断trace的system call是谁，我一开始的做法是根据这个`mask >> 1`，重复进行这样的操作，知道`mask`为1，得出syscall number，这样做是有问题的，这是对trace后面的system call的数量没有一个清晰的认识。
2. 另外`trace`还要求打印trace的system call返回的结果，我的想法是求出是哪个system call，然后立马在kernel中运行它的`sys_xxx()`求出返回的结果，这又进入了另一个误区：trace结束之后应当立即返回，不然为什么`trace.c`后面有个`while` loop执行`exec()`。