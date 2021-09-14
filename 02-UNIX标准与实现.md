### UNIX 标准与实现



`ANSI` 是美国国家标准学会（American National Standards Institute）

`ISO` 国际标准化组织（International Organization for Standardization）

`IEC` 是国际电子技术委员会（International Electrotechnical Commission）

 `IEEE` 是电气和电子工程师学会（Institute of Electrical and Electronics Engineers）

`POSIX` 是可移植操作系统接口（Portable Operating System Interface）

ANSI 是 ISO 中代表美国的成员。

#### UNIX 标准化

**ISO C**

ISO C标准现在由ISO/IEC的C程序设计语言国际标准工作组维护和开发。ISO C 标准的意图是提供 C 程序的可移植性，使其能适合于大量不同的操作系统，而不只是适合 UNIX 系统。



按照该标准定义的各个头文件可将 ISO C 库分成 24 个区。POSIX.1 标准包括这些
头文件以及另外一些头文件。

所有这些头文件在4 种 UNIX 实现（`FreeBSD 8.0`、`Linux 3.2.0`、`Mac OS X 10.6.8` 和 `Solaris 10`）中都支持。

```shell
<assert.h> 		验证程序断言
<complex.h>		复数算术运算支持
<ctype.h>		字符分类和映射支持
<errno.h> 		出错码
<fenv.h>		浮点环境
<float.h>		浮点常量及特性
<inttypes.h>	整型格式变换
<iso646.h>		赋值、关系及一元操作符宏
<limits.h>		实现常量
<locale.h>		本地化类别及相关定义
<math.h> 		数学函数、类型声明及常量
<setjmp.h>		非局部 goto
<signal.h> 		信号
<stdarg.h>		可变长度参数表
<stdbool.h>		布尔类型和值
<stddef.h> 		标准定义
<stdint.h>		整型
<stdio.h>		标准 I/O 库
<stdlib.h>		实用函数
<string.h>		字符串操作
<tgmath.h>		通用类型数学宏
<time.h>		时间和日期
<wchar.h>		扩充的多字节和宽字符支持
<wctype.h> 		宽字符分类和映射支持
```

#### IEEE POSIX

POSIX 是一个最初由 IEEE 制订的标准族。POSIX 指的是可移植操作系统接口。

它原来指的只是 IEEE 标准 1003.1-1988（操作系统接口），后来则扩展成包括很多标记为 1003 的标准及标准草案。

由于 1003.1 标准说明了一个接口（interface）而不是一种实现（implementation），所以并不区分系统调用和库函数。所有在标准中的例程都被称为函数。



 POSIX.1 指定的必需的和可选的头文件。因为 POSIX.1 包含了 `ISO C` 标准库函数，所以它还需要引入`ISO C`列出的各个头文件。

```shell
<aio.h>			异步 I/O
<cpio.h>		cpio 归档值
<dirent.h> 		目录项
<dlfcn.h>		动态链接
<fcntl.h>		文件控制
<fnmatch.h>		文件名匹配类型
<glob.h>		路径名模式匹配与生成
<grp.h>			组文件
<iconv.h>		代码集变换实用程序
<langinfo.h>	语言信息常量
<monetary.h>	货币类型与函数
<netdb.h>		网络数据库操作
<nl_types.h>	消息类
<poll.h>		投票函数
<pthread.h>		线程
<pwd.h> 		口令文件
<regex.h>		正则表达式
<sched.h>		执行调度
<semaphore.h>	信号量
<strings.h>		字符串操作
<tar.h>			tar 归档值
<termios.h>		终端 I/O
<unistd.h>		符号常量
<wordexp.h>		字扩充类型

<arpa/inet.h>	因特网定义
<net/if.h> 		套接字本地接口
<netinet/in.h>	因特网地址族
<netinet/tcp.h>

<sys/mman.h>	存储管理声明
<sys/select.h>	select 函数
<sys/socket.h>	套接字接口
<sys/stat.h> 	文件状态
<sys/statvfs.h>	文件系统信息
<sys/times.h>	进程时间
<sys/types.h>	基本系统数据类型
<sys/un.h>		UNIX域套接字定义
<sys/utsname.h>	系统名
<sys/wait.h>	进程控制
```

本书中描述了 POSIX.1 2008 年版，其接口分成两部分：必需部分和可选部分。可选接口部分按功能又进一步分成 40 个功能分区。下面为按各自的选项码总结了包含未弃用的编程接口。

选项码是能够表述标准的 2～3 个字母的缩写，用以标识属于各个功能分区的接口，其中的接口依赖于特定选项的支持。

```shell
<fmtmsg.h> 		消息显示结构
<ftw.h>			文件树漫游
<libgen.h>		路径名管理函数
<ndbm.h>		数据库操作
<search.h>		搜索表
<syslog.h>		系统出错日志记录
<utmpx.h> 		用户账户数据库

<sys/ipc.h> 	IPC
<sys/msg.h>		XSI 消息队列
<sys/resource.h>	资源操作
<sys/sem.h>		XSI 信号量
<sys/shm.h>		XSI 共享存储
<sys/time.h> 	时间类型
<sys/uio.h> 	矢量 I/O 操作
```

POSIX 标准定义的 XSI 可选头文件

```shell
<mqueue.h> 		消息队列
<spawn.h>		实时 spawn 接口
```

POSIX 标准定义的可选头文件

```shell
选项码	SUS 强制的	符号常量				说明
ADV 		_POSIX_ADVISORY_INFO 	建议性信息（实时）
CPT 		_POSIX_CPUTIME 			进程 CPU 时间时钟（实时）
FSC 	• 	_POSIX_FSYNC 			文件同步
IP6 		_POSIX_IPV6 			IPv6 接口
ML 			_POSIX_MEMLOCK 			进程存储区加锁（实时）
MLR 		_POSIX_MEMLOCK_RANGE 	存储区域加锁（实时）
MON 		_POSIX_MONOTONIC_CLOCK 	单调时钟（实时）
MSG 		_POSIX_MESSAGE_PASSING 消息传送（实时）
MX 			__STDC_IEC_559__ 		IEC 60559 浮点选项
PIO 		_POSIX_PRIORITIZED_IO 	优先输入和输出
PS 			_POSIX_PRIORITIZED_SCHEDULING 进程调度（实时）
RPI 		_POSIX_THREAD_ROBUST_PRIO_INHERIT 健壮的互斥量优先权继承（实时）
RPP 		_POSIX_THREAD_ROBUST_PRIO_PROTECT 健壮的互斥量优先权保护（实时）
RS 			_POSIX_RAW_SOCKETS 原始套接字
SHM 		_POSIX_SHARED_MEMORY_OBJECTS 共享存储对象（实时）
SIO 		_POSIX_SYNCHRONIZED_IO 同步输入和输出（实时）
SPN 		_POSIX_SPAWN 产生（实时）
SS 			_POSIX_SPORADIC_SERVER 进程阵发性服务器（实时）
TCT 		_POSIX_THREAD_CPUTIME 线程 CPU 时间时钟（实时）
TPI 		_POSIX_THREAD_PRIO_INHERIT 非键壮的互斥量优先权继承（实时）
TPP 		_POSIX_THREAD_PRIO_PROTECT 非键壮的互斥量优先权保护（实时）
TPS 		_POSIX_THREAD_PRIORITY_SCHEDULING 线程执行调度（实时）
TSA 	• 	_POSIX_THREAD_ATTR_STACKADDR 线程栈地址属性
TSH 	• 	_POSIX_THREAD_PROCESS_SHARED 线程进程共享同步
TSP 		_POSIX_THREAD_SPORADIC_SERVER 线程阵发性服务器（实时）
TSS 	• 	_POSIX_THREAD_ATTR_STACKSIZE 线程栈长度属性
TYM 		_POSIX_TYPED_MEMORY_OBJECTS 类型存储对象（实时）
XSI 	• 	_XOPEN_UNIX X/Open 扩充接口
```



#### Single UNIX Specification

`Single UNIX Specification`（SUS，单一 UNIX 规范）是 POSIX.1 标准的一个超集，它定义了一些附加接口扩展了 POSIX.1 规范提供的功能。

POSIX.1 相当于 Single UNIX Specification 中的基本规范部分。



POSIX.1 中的 X/Open 系统接口（`X/Open System Interface，XSI`）选项描述了可选的接口，也定义了遵循 XSI（XSI conforming）的实现必须支持 POSIX.1 的哪些可选部分。

这些必须支持的部分包括：文件同步、线程栈地址和长度属性、线程进程共享同步以及_XOPEN_UNIX 符号常量（在上面它们被加上“SUS 强制的”的标记）。只有遵循 XSI 的实现才能称为 UNIX系统。







#### UNIX 系统实现

上面3 个由各自独立的组织所制定的标准：`ISO C`、`IEEE POSIX` 以及 `Single UNIX Specification`。但是，标准只是接口的规范。



UNIX 系统家族树，从这棵树上演进出以下 3 个分支。

+ `AT&T` 分支，从此引出了系统 III 和系统 V（被称为 UNIX 的商用版本）。
+ 加州大学伯克利分校分支，从此引出 `4.xBSD` 实现。
+ 由 AT&T 贝尔实验室的计算科学研究中心不断开发的 `UNIX 研究版本`，从此引出 UNIX分时系统第 8 版、第 9 版，终止于 1990 年的第 10 版。



**SVR4**

`SVR4（UNIX System V Release 4）`是 AT&T 的 UNIX 系统实验室（UNIX System Laboratories，USL，其前身是 AT&T 的 UNIX Software Operation）的产品。



**4.4BSD**

`BSD（Berkeley Software Distribution）`是由加州大学伯克利分校的计算机系统研究组（CSRG）研究开发和分发的。

最初的 BSD 系统包含了 AT&T 专有的源代码，它们需要 AT&T 许可证。为了获得 BSD 系统的源代码，首先需要持有 AT&T 的 UNIX 源代码许可证。

4.4BSD-Lite 立即于 1994 年发布，并且不再需要具有 UNIX 源代码使用许可证就可以使用它。

**FreeBSD**

FreeBSD 基于 4.4BSD-Lite 操作系统。在加州大学伯克利分校的 CSRG 决定终止其在 UNIX操作系统的 BSD 版本的研发工作，而且 386BSD 项目被忽视很长时间之后，为了继续坚持 BSD系列，形成了 FreeBSD 项目。

**Linux**

Linux 是一种提供类似于 UNIX 的丰富编程环境的操作系统，在 GNU 公用许可证的指导下，Linux 是免费使用的。



**Mac OS X**

Mac OS X 使用了完全不同的技术。其核心操作系统称为“Darwin”，它
基于 Mach 内核（Accetta 等[1986]）、FreeBSD 操作系统以及具有面向对象框架的驱动和其他内核扩展的结合。



**Solaris**

Solaris 是由 Sun Microsystems（现为 Oracle）开发的 UNIX 系统版本。它基于 SVR4，在超过15 年的时间里，Sun Microsystems 的工程师对其功能不断增强。它是唯一在商业上取得成功的SVR4 后裔，并被正式验证为 UNIX 系统。



#### 限制

以下两种类型的限制是必需的。

+ 编译时限制（例如，短整型的最大值是什么？）
+ 运行时限制（例如，文件名有多少个字符？）

编译时限制可在头文件中定义。程序在编译时可以包含这些头文件。但是，运行时限制则要求进程调用一个函数获得限制值。



另外，某些限制在一个给定的实现中可能是固定的（因此可以静态地在一个头文件中定义），而在另一个实现中则可能是变动的（需要有一个运行时函数调用）。

这种类型限制的一个例子是文件名的最大字符数。SVR4 之前的系统V 由于历史原因只允许文件名最多包含14 个字符，而源于BSD 的系统则将此增加为255。

文件名的最大长度依赖于该文件处于何种文件系统，为了解决这类问题，提供了以下 3 种限制。

+ 编译时限制（头文件）。
+ 与文件或目录无关的运行时限制（`sysconf `函数 ）
+ 与文件或目录有关的运行时限制（`pathconf `和 `fpathconf` 函数）

#### ISO C 限制

ISO C 定义的所有编译时限制都列在头文件`<limits.h>`中。

虽然ISO C 标准规定了整型数据类型可接受的最小值，但POSIX.1 对C 标准进行了扩充。

#### POSIX 限制

POSIX.1 定义了很多涉及操作系统实现限制的常量，遗憾的是，这是 POSIX.1 中最令人迷惑不解的部分之一。

虽然 POSIX.1 定义了大量限制和常量，我们只关心与基本 POSIX.1 接口有关的
部分。这些限制和常量分成下列 7 类。

1. 数值限制：`LONG_BIT`、`SSIZE_MAX` 和 `WORD_BIT`。
2. 最小值: 
3. 最大值：`_POSIX_CLOCKRES_MIN`
4. 运行时可以增加的值：`CHARCLASS_NAME_MAX`、`COLL_WEIGHTS_MAX`、`LINE_MAX`、`NGROUPS_MAX` 和 `RE_DUP_MAX`。
5. 运行时不变值（可能不确定）
6. 其他不变值：`NL_ARGMAX`、`NL_MSGMAX`、`NL_SETMAX` 和 `NL_TEXTMAX`。
7. 路径名可变值：`FILESIZEBITS`、`LINK_MAX`、`MAX_CANON`、`MAX_INPUT`、`NAME_MAX`、`PATH_MAX`、`PIPE_BUF` 和`SYMLINK_MAX`。

`<limits.h>`中的 POSIX.1 最小值

```shell
_POSIX_ARG_MAX 			exec 函数的参数长度 4 096
_POSIX_CHILD_MAX 		每个实际用户 ID 的子进程数 25
_POSIX_DELAYTIMER_MAX 	定时器最大超限运行次数 32
_POSIX_HOST_NAME_MAX 	gethostname 函数返回的主机名长度 255
_POSIX_LINK_MAX 		至一个文件的链接数 8
_POSIX_LOGIN_NAME_MAX 	登录名的长度 9
_POSIX_MAX_CANON 		终端规范输入队列的字节数 255
_POSIX_MAX_INPUT 		终端输入队列的可用空间 255
_POSIX_NAME_MAX 		文件名中的字节数，不包括终止 null 字节 14
_POSIX_NGROUPS_MAX 		每个进程同时添加的组 ID 数 8
_POSIX_OPEN_MAX 		每个进程的打开文件数 20
_POSIX_PATH_MAX 		路径名中的字节数，包括终止 null 字节 256
_POSIX_PIPE_BUF 		能原子地写到一个管道中的字节数 512
_POSIX_RE_DUP_MAX
	当使用间隔表示法{m,n}时，regexec 和 regcomp 函数允许
的基本正则表达式重复发生次数
255
_POSIX_RTSIG_MAX 		为应用预留的实时信号编号个数 8
_POSIX_SEM_NSEMS_MAX 	一个进程可以同时使用的信号量个数 256
_POSIX_SEM_VALUE_MAX 	信号量可持有的值 32 767
_POSIX_SIGQUEUE_MAX 	一个进程可发送和挂起的排队信号的个数 32
_POSIX_SSIZE_MAX 		能存在 ssize_t 对象中的值 32 767
_POSIX_STREAM_MAX 		一个进程能同时打开的标准 I/O 流数 8
_POSIX_SYMLINK_MAX 		符号链接中的字节数 255
_POSIX_SYMLOOP_MAX 		在解析路径名时，可遍历的符号链接数 8
_POSIX_TIMER_MAX 		每个进程的定时器数目 32
_POSIX_TTY_NAME_MAX 	终端设备名长度，包括终止 null 字节 9
_POSIX_TZNAME_MAX 		时区名字节数 6
```



目前在大多数 UNIX 系统中，每个进程可同时打开的文件数远远超过 20。另外，`_POSIX_PATH_MAX` 的最小限制值为 255，这太小了，路径名可能会超过这一限制。这意味着在编译时不能使用`_POSIX_OPEN_MAX` 和`_POSIX_PATH_MAX` 这两个常量作为数组长度。



`<limits.h>`中的 POSIX.1 运行时不变值

```shell
ARG_MAX 		exec 函数族的参数最大长度 			_POSIX_ARG_MAX
ATEXIT_MAX 		可用 atexit 函数登记的最大函数个数 		32
CHILD_MAX 		每个实际用户 ID 的子进程最大个数 		_POSIX_CHILD_MAX
DELAYTIMER_MAX 	定时器最大超限运行次数 				_POSIX_DELAYTIMER_MAX
HOST_NAME_MAX 	gethostname 返回的主机名长度 		_POSIX_HOST_NAME_MAX
LOGIN_NAME_MAX 登录名最大长度 						_POSIX_LOGIN_NAME_MAX
OPEN_MAX 		赋予新建文件描述符的最大值+1 		_POSIX_OPEN_MAX
PAGESIZE 		系统内存页大小（以字节为单位） 		1
RTSIG_MAX 		为应用程序预留的实时信号的最大个数 		_POSIX_RTSIG_MAX
SEM_NSEMS_MAX 	一个进程可使用的信号量最大个数 		_POSIX_SEM_NSEMS_MAX
SEM_VALUE_MAX 	信号量的最大值 					_POSIX_SEM_VALUE_MAX
SIGQUEUE_MAX 	一个进程可排队信号的最大个数 			_POSIX_SIGQUEUE_MAX
STREAM_MAX 		一个进程一次可打开的标准 I/O 流的最大个数 _POSIX_STREAM_MAX
SYMLOOP_MAX 	路径解析过程中可访问的符号链接数 		_POSIX_SYMLOOP_MAX
TIMER_MAX 		一个进程的定时器最大个数 				_POSIX_TIMER_MAX
TTY_NAME_MAX 	终端设备名长度，其中包括终止的 null 字节 _POSIX_TTY_NAME_MAX
TZNAME_MAX 		时区名的字节数 					_POSIX_TZNAME_MAX
```



#### XSI 限制

XSI 定义了代表实现限制的几个常量。

+ 最小值
+ 运行时不变值（可能不确定）：`IOV_MAX` 和 `PAGE_SIZE`。

`<limits.h>` 中的 XSI 最小值

```shell
											最小可接受值 典型值
NL_LANGMAX 		在 LANG 环境变量中最大字节数 		14 		14
NZERO 			默认进程优先级 					20 		20
_XOPEN_IOV_MAX 	readv 或 writev 可使用的最多 iovec 结构个数 16 16
_XOPEN_NAME_MAX 文件名中的字节数 					255 	255
_XOPEN_PATH_MAX 路径名中的字节数 					1024 	1024
```



#### 函数 sysconf 、pathconf 和 fpathconf

```c
#include <unistd.h>
long sysconf(int name);
long pathconf(const char *pathname, int name);
log fpathconf(int fd, int name);

// 所有函数返回值：若成功，返回相应值；若出错，返回-1（见后）
```

以`_SC_`开始的常量用作标识运行时限制的 `sysconf` 参数。

以`_PC_`开始的常量用作标识运行时限制的`pathconf` 或`fpathconf` 参数。

如果`name` 参数并不是一个合适的常量，这3 个函数都返回`−1`，并把 `errno` 置为 `EINVAL`。



#### 功能测试宏

头文件定义了很多 POSIX.1 和 XSI 符号。但是除了 POSIX.1 和 XSI 定义外，大多数实现在这些头文件中也加入了它们自己的定义。

如果在编译一个程序时，希望它只与 POSIX的定义相关，而不与任何实现定义的常量冲突，那么就需要定义常量`_POSIX_C_SOURCE`。一旦定义了`_POSIX_C_SOURCE`，所有 POSIX.1 头文件都使用此常量来排除任何实现专有的定义。

常量`_POSIX_C_SOURCE` 及`_XOPEN_SOURCE` 被称为`功能测试宏（feature test macro）`。

当要使用它们时，通常在 cc 命令行中以下列方式定义：

```c
cc -D_POSIX_C_SOURCE=200809L file.c

// 源文件中定义
// #define_POSIX_C_SOURCE 200809L
```

为使 SUSv4 的 XSI 选项可由应用程序使用，需将常量`_XOPEN_SOURCE` 定义为 700。



#### 基本系统数据类型

历史上，某些 UNIX 系统变量已与某些 C 数据类型联系在一起。

头文件`<sys/types.h>`中定义了某些与实现有关的数据类型，它们被称为`基本系统数据类型（primitive system data type）`。

常用的基本系统数据类型

```shell
clock_t 	时钟滴答计数器（进程时间）
comp_t 		压缩的时钟滴答（POSIX.1 未定义；）
dev_t 		设备号（主和次）
fd_set 		文件描述符集
fpos_t 		文件位置
gid_t 		数值组 ID
ino_t i 	节点编号
mode_t 		文件类型，文件创建模式
nlink_t 	目录项的链接计数
off_t 		文件长度和偏移量（带符号的）
pid_t 		进程 ID 和进程组 ID（带符号的）
pthread_t 	线程 ID
ptrdiff_t 	两个指针相减的结果（带符号的）
rlim_t 		资源限制
sig_atomic_t 能原子性地访问的数据类型
sigset_t 	信号集
size_t 		对象（如字符串）长度（不带符号的）
ssize_t 	返回字节计数的函数（带符号的）
time_t 		日历时间的秒计数器
uid_t 		数值用户 ID
wchar_t 	能表示所有不同的字符码
```





​	# end

