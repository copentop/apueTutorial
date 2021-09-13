#include "apue.h"

#include <limits.h>
#include <errno.h>		/* for definition of errno */
#include <stdarg.h>		/* ISO C variable aruments */
#include <syslog.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <time.h>
#include <termios.h>




// pathalloc

#ifdef	PATH_MAX
static long	pathmax = PATH_MAX;
#else
static long	pathmax = 0;
#endif

static long	posix_version = 0;
static long	xsi_version = 0;

/* If PATH_MAX is indeterminate, no guarantee this is adequate */
#define	PATH_MAX_GUESS	1024

char *
path_alloc(size_t *sizep) /* also return allocated size, if nonnull */
{
	char	*ptr;
	size_t	size;

	if (posix_version == 0)
		posix_version = sysconf(_SC_VERSION);

	if (xsi_version == 0)
		xsi_version = sysconf(_SC_XOPEN_VERSION);

	if (pathmax == 0) {		/* first time through */
		errno = 0;
		if ((pathmax = pathconf("/", _PC_PATH_MAX)) < 0) {
			if (errno == 0)
				pathmax = PATH_MAX_GUESS;	/* it's indeterminate */
			else
				err_sys("pathconf error for _PC_PATH_MAX");
		} else {
			pathmax++;		/* add one since it's relative to root */
		}
	}

	/*
	 * Before POSIX.1-2001, we aren't guaranteed that PATH_MAX includes
	 * the terminating null byte.  Same goes for XPG3.
	 */
	if ((posix_version < 200112L) && (xsi_version < 4))
		size = pathmax + 1;
	else
		size = pathmax;

	if ((ptr = malloc(size)) == NULL)
		err_sys("malloc error for pathname");

	if (sizep != NULL)
		*sizep = size;
	return(ptr);
}

// end pathalloc


// openmax

#ifdef	OPEN_MAX
static long	openmax = OPEN_MAX;
#else
static long	openmax = 0;
#endif

/*
 * If OPEN_MAX is indeterminate, this might be inadequate.
 */
#define	OPEN_MAX_GUESS	256

long
open_max(void)
{
	if (openmax == 0) {		/* first time through */
		errno = 0;
		if ((openmax = sysconf(_SC_OPEN_MAX)) < 0) {
			if (errno == 0)
				openmax = OPEN_MAX_GUESS;	/* it's indeterminate */
			else
				err_sys("sysconf error for _SC_OPEN_MAX");
		}
	}
	return(openmax);
}

// end openmax

// setfd

int
set_cloexec(int fd)
{
	int		val;

	if ((val = fcntl(fd, F_GETFD, 0)) < 0)
		return(-1);

	val |= FD_CLOEXEC;		/* enable close-on-exec */

	return(fcntl(fd, F_SETFD, val));
}

// end setfd

// clrfl

void
clr_fl(int fd, int flags)
				/* flags are the file status flags to turn off */
{
	int		val;

	if ((val = fcntl(fd, F_GETFL, 0)) < 0)
		err_sys("fcntl F_GETFL error");

	val &= ~flags;		/* turn flags off */

	if (fcntl(fd, F_SETFL, val) < 0)
		err_sys("fcntl F_SETFL error");
}

// end clrfl


// setfl

void
set_fl(int fd, int flags) /* flags are file status flags to turn on */
{
	int		val;

	if ((val = fcntl(fd, F_GETFL, 0)) < 0)
		err_sys("fcntl F_GETFL error");

	val |= flags;		/* turn on flags */

	if (fcntl(fd, F_SETFL, val) < 0)
		err_sys("fcntl F_SETFL error");
}

// end setfl

// prexit


void
pr_exit(int status)
{
	if (WIFEXITED(status))
		printf("normal termination, exit status = %d\n",
				WEXITSTATUS(status));
	else if (WIFSIGNALED(status))
		printf("abnormal termination, signal number = %d%s\n",
				WTERMSIG(status),
#ifdef	WCOREDUMP
				WCOREDUMP(status) ? " (core file generated)" : "");
#else
				"");
#endif
	else if (WIFSTOPPED(status))
		printf("child stopped, signal number = %d\n",
				WSTOPSIG(status));
}

// end preit


// prmask


void
pr_mask(const char *str)
{
	sigset_t	sigset;
	int			errno_save;

	errno_save = errno;		/* we can be called by signal handlers */
	if (sigprocmask(0, NULL, &sigset) < 0) {
		err_ret("sigprocmask error");
	} else {
		printf("%s", str);
		if (sigismember(&sigset, SIGINT))
			printf(" SIGINT");
		if (sigismember(&sigset, SIGQUIT))
			printf(" SIGQUIT");
		if (sigismember(&sigset, SIGUSR1))
			printf(" SIGUSR1");
		if (sigismember(&sigset, SIGALRM))
			printf(" SIGALRM");

		/* remaining signals can go here  */

		printf("\n");
	}

	errno = errno_save;		/* restore errno */
}

// end prmask


// signalintr


Sigfunc *
signal_intr(int signo, Sigfunc *func)
{
	struct sigaction	act, oact;

	act.sa_handler = func;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
#ifdef	SA_INTERRUPT
	act.sa_flags |= SA_INTERRUPT;
#endif
	if (sigaction(signo, &act, &oact) < 0)
		return(SIG_ERR);
	return(oact.sa_handler);
}

// end signalintr


// daemonize


void
daemonize(const char *cmd)
{
	int i, fd0, fd1, fd2;
	pid_t pid;
	struct rlimit rl;
	struct sigaction sa;

	/*
	 * Clear file creation mask.
	 */
	umask(0);

	/*
	 * Get maximum number of file descriptors.
	 */
	if (getrlimit(RLIMIT_NOFILE, &rl) < 0)
		err_quit("%s: can't get file limit", cmd);

	/*
	 * Become a session leader to lose controlling TTY.
	 */
	if ((pid = fork()) < 0)
		err_quit("%s: can't fork", cmd);
	else if (pid != 0) /* parent */
		exit(0);
	setsid();

	/*
	 * Ensure future opens won't allocate controlling TTYs.
	 */
	sa.sa_handler = SIG_IGN;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	if (sigaction(SIGHUP, &sa, NULL) < 0)
		err_quit("%s: can't ignore SIGHUP", cmd);
	if ((pid = fork()) < 0)
		err_quit("%s: can't fork", cmd);
	else if (pid != 0) /* parent */
		exit(0);

	/*
	 * Change the current working directory to the root so
	 * we won't prevent file systems from being unmounted.
	 */
	if (chdir("/") < 0)
		err_quit("%s: can't change directory to /", cmd);

	/*
	 * Close all open file descriptors.
	 */
	if (rl.rlim_max == RLIM_INFINITY)
		rl.rlim_max = 1024;
	for (i = 0; i < rl.rlim_max; i++)
		close(i);

	/*
	 * Attach file descriptors 0, 1, and 2 to /dev/null.
	 */
	fd0 = open("/dev/null", O_RDWR);
	fd1 = dup(0);
	fd2 = dup(0);

	/*
	 * Initialize the log file.
	 */
	openlog(cmd, LOG_CONS, LOG_DAEMON);
	if (fd0 != 0 || fd1 != 1 || fd2 != 2) {
		syslog(LOG_ERR, "unexpected file descriptors %d %d %d",
		  fd0, fd1, fd2);
		exit(1);
	}
}

// end daemonize


// sleepus


void
sleep_us(unsigned int nusecs)
{
	struct timeval	tval;

	tval.tv_sec = nusecs / 1000000;
	tval.tv_usec = nusecs % 1000000;
	select(0, NULL, NULL, NULL, &tval);
}

// end sleepus

// readn

ssize_t             /* Read "n" bytes from a descriptor  */
readn(int fd, void *ptr, size_t n)
{
	size_t		nleft;
	ssize_t		nread;

	nleft = n;
	while (nleft > 0) {
		if ((nread = read(fd, ptr, nleft)) < 0) {
			if (nleft == n)
				return(-1); /* error, return -1 */
			else
				break;      /* error, return amount read so far */
		} else if (nread == 0) {
			break;          /* EOF */
		}
		nleft -= nread;
		ptr   += nread;
	}
	return(n - nleft);      /* return >= 0 */
}

// end readn

// writen


ssize_t             /* Write "n" bytes to a descriptor  */
writen(int fd, const void *ptr, size_t n)
{
	size_t		nleft;
	ssize_t		nwritten;

	nleft = n;
	while (nleft > 0) {
		if ((nwritten = write(fd, ptr, nleft)) < 0) {
			if (nleft == n)
				return(-1); /* error, return -1 */
			else
				break;      /* error, return amount written so far */
		} else if (nwritten == 0) {
			break;
		}
		nleft -= nwritten;
		ptr   += nwritten;
	}
	return(n - nleft);      /* return >= 0 */
}

// end writen


// spipe

/*
 * Returns a full-duplex pipe (a UNIX domain socket) with
 * the two file descriptors returned in fd[0] and fd[1].
 */
int
fd_pipe(int fd[2])
{
	return(socketpair(AF_UNIX, SOCK_STREAM, 0, fd));
}

// end spipe

// recvfd

/* size of control buffer to send/recv one file descriptor */
#define	CONTROLLEN	CMSG_LEN(sizeof(int))

#ifdef LINUX
#define RELOP <
#else
#define RELOP !=
#endif

static struct cmsghdr	*cmptr = NULL;		/* malloc'ed first time */

/*
 * Receive a file descriptor from a server process.  Also, any data
 * received is passed to (*userfunc)(STDERR_FILENO, buf, nbytes).
 * We have a 2-byte protocol for receiving the fd from send_fd().
 */
int
recv_fd(int fd, ssize_t (*userfunc)(int, const void *, size_t))
{
	int				newfd, nr, status;
	char			*ptr;
	char			buf[MAXLINE];
	struct iovec	iov[1];
	struct msghdr	msg;

	status = -1;
	for ( ; ; ) {
		iov[0].iov_base = buf;
		iov[0].iov_len  = sizeof(buf);
		msg.msg_iov     = iov;
		msg.msg_iovlen  = 1;
		msg.msg_name    = NULL;
		msg.msg_namelen = 0;
		if (cmptr == NULL && (cmptr = malloc(CONTROLLEN)) == NULL)
			return(-1);
		msg.msg_control    = cmptr;
		msg.msg_controllen = CONTROLLEN;
		if ((nr = recvmsg(fd, &msg, 0)) < 0) {
			err_ret("recvmsg error");
			return(-1);
		} else if (nr == 0) {
			err_ret("connection closed by server");
			return(-1);
		}

		/*
		 * See if this is the final data with null & status.  Null
		 * is next to last byte of buffer; status byte is last byte.
		 * Zero status means there is a file descriptor to receive.
		 */
		for (ptr = buf; ptr < &buf[nr]; ) {
			if (*ptr++ == 0) {
				if (ptr != &buf[nr-1])
					err_dump("message format error");
 				status = *ptr & 0xFF;	/* prevent sign extension */
 				if (status == 0) {
					if (msg.msg_controllen RELOP CONTROLLEN)
						err_dump("status = 0 but no fd");
					newfd = *(int *)CMSG_DATA(cmptr);
				} else {
					newfd = -status;
				}
				nr -= 2;
			}
		}
		if (nr > 0 && (*userfunc)(STDERR_FILENO, buf, nr) != nr)
			return(-1);
		if (status >= 0)	/* final data has arrived */
			return(newfd);	/* descriptor, or -status */
	}
}

// end recvfd


// sendfd

static struct cmsghdr	*send_cmptr = NULL;	/* malloc'ed first time */

/*
 * Pass a file descriptor to another process.
 * If fd<0, then -fd is sent back instead as the error status.
 */
int
send_fd(int fd, int fd_to_send)
{
	struct iovec	iov[1];
	struct msghdr	msg;
	char			buf[2];	/* send_fd()/recv_fd() 2-byte protocol */

	iov[0].iov_base = buf;
	iov[0].iov_len  = 2;
	msg.msg_iov     = iov;
	msg.msg_iovlen  = 1;
	msg.msg_name    = NULL;
	msg.msg_namelen = 0;

	if (fd_to_send < 0) {
		msg.msg_control    = NULL;
		msg.msg_controllen = 0;
		buf[1] = -fd_to_send;	/* nonzero status means error */
		if (buf[1] == 0)
			buf[1] = 1;	/* -256, etc. would screw up protocol */
	} else {
		if (send_cmptr == NULL && (send_cmptr = malloc(CONTROLLEN)) == NULL)
			return(-1);
		send_cmptr->cmsg_level  = SOL_SOCKET;
		send_cmptr->cmsg_type   = SCM_RIGHTS;
		send_cmptr->cmsg_len    = CONTROLLEN;
		msg.msg_control    = send_cmptr;
		msg.msg_controllen = CONTROLLEN;
		*(int *)CMSG_DATA(send_cmptr) = fd_to_send;		/* the fd to pass */
		buf[1] = 0;		/* zero status means OK */
	}

	buf[0] = 0;			/* null byte flag to recv_fd() */
	if (sendmsg(fd, &msg, 0) != 2)
		return(-1);
	return(0);
}

// end sendfd

// senderror

/*
 * Used when we had planned to send an fd using send_fd(),
 * but encountered an error instead.  We send the error back
 * using the send_fd()/recv_fd() protocol.
 */
int
send_err(int fd, int errcode, const char *msg)
{
	int		n;

	if ((n = strlen(msg)) > 0)
		if (writen(fd, msg, n) != n)	/* send the error message */
			return(-1);

	if (errcode >= 0)
		errcode = -1;	/* must be negative */

	if (send_fd(fd, errcode) < 0)
		return(-1);

	return(0);
}

// end senderror

// servlisten

#define QLEN	10

/*
 * Create a server endpoint of a connection.
 * Returns fd if all OK, <0 on error.
 */
int
serv_listen(const char *name)
{
	int					fd, len, err, rval;
	struct sockaddr_un	un;

	if (strlen(name) >= sizeof(un.sun_path)) {
		errno = ENAMETOOLONG;
		return(-1);
	}

	/* create a UNIX domain stream socket */
	if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
		return(-2);

	unlink(name);	/* in case it already exists */

	/* fill in socket address structure */
	memset(&un, 0, sizeof(un));
	un.sun_family = AF_UNIX;
	strcpy(un.sun_path, name);
	len = offsetof(struct sockaddr_un, sun_path) + strlen(name);

	/* bind the name to the descriptor */
	if (bind(fd, (struct sockaddr *)&un, len) < 0) {
		rval = -3;
		goto errout;
	}

	if (listen(fd, QLEN) < 0) {	/* tell kernel we're a server */
		rval = -4;
		goto errout;
	}
	return(fd);

errout:
	err = errno;
	close(fd);
	errno = err;
	return(rval);
}

// end servlisten


// servaccept

#define	STALE	30	/* client's name can't be older than this (sec) */

/*
 * Wait for a client connection to arrive, and accept it.
 * We also obtain the client's user ID from the pathname
 * that it must bind before calling us.
 * Returns new fd if all OK, <0 on error
 */
int
serv_accept(int listenfd, uid_t *uidptr)
{
	int					clifd, err, rval;
	socklen_t			len;
	time_t				staletime;
	struct sockaddr_un	un;
	struct stat			statbuf;
	char				*name;

	/* allocate enough space for longest name plus terminating null */
	if ((name = malloc(sizeof(un.sun_path + 1))) == NULL)
		return(-1);
	len = sizeof(un);
	if ((clifd = accept(listenfd, (struct sockaddr *)&un, &len)) < 0) {
		free(name);
		return(-2);		/* often errno=EINTR, if signal caught */
	}

	/* obtain the client's uid from its calling address */
	len -= offsetof(struct sockaddr_un, sun_path); /* len of pathname */
	memcpy(name, un.sun_path, len);
	name[len] = 0;			/* null terminate */
	if (stat(name, &statbuf) < 0) {
		rval = -3;
		goto errout;
	}

#ifdef	S_ISSOCK	/* not defined for SVR4 */
	if (S_ISSOCK(statbuf.st_mode) == 0) {
		rval = -4;		/* not a socket */
		goto errout;
	}
#endif

	if ((statbuf.st_mode & (S_IRWXG | S_IRWXO)) ||
		(statbuf.st_mode & S_IRWXU) != S_IRWXU) {
		  rval = -5;	/* is not rwx------ */
		  goto errout;
	}

	staletime = time(NULL) - STALE;
	if (statbuf.st_atime < staletime ||
		statbuf.st_ctime < staletime ||
		statbuf.st_mtime < staletime) {
		  rval = -6;	/* i-node is too old */
		  goto errout;
	}

	if (uidptr != NULL)
		*uidptr = statbuf.st_uid;	/* return uid of caller */
	unlink(name);		/* we're done with pathname now */
	free(name);
	return(clifd);

errout:
	err = errno;
	close(clifd);
	free(name);
	errno = err;
	return(rval);
}

// end servaccet

// cliconn

#define	CLI_PATH	"/var/tmp/"
#define	CLI_PERM	S_IRWXU			/* rwx for user only */

/*
 * Create a client endpoint and connect to a server.
 * Returns fd if all OK, <0 on error.
 */
int
cli_conn(const char *name)
{
	int					fd, len, err, rval;
	struct sockaddr_un	un, sun;
	int					do_unlink = 0;

	if (strlen(name) >= sizeof(un.sun_path)) {
		errno = ENAMETOOLONG;
		return(-1);
	}

	/* create a UNIX domain stream socket */
	if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
		return(-1);

	/* fill socket address structure with our address */
	memset(&un, 0, sizeof(un));
	un.sun_family = AF_UNIX;
	sprintf(un.sun_path, "%s%05ld", CLI_PATH, (long)getpid());
printf("file is %s\n", un.sun_path);
	len = offsetof(struct sockaddr_un, sun_path) + strlen(un.sun_path);

	unlink(un.sun_path);		/* in case it already exists */
	if (bind(fd, (struct sockaddr *)&un, len) < 0) {
		rval = -2;
		goto errout;
	}
	if (chmod(un.sun_path, CLI_PERM) < 0) {
		rval = -3;
		do_unlink = 1;
		goto errout;
	}

	/* fill socket address structure with server's address */
	memset(&sun, 0, sizeof(sun));
	sun.sun_family = AF_UNIX;
	strcpy(sun.sun_path, name);
	len = offsetof(struct sockaddr_un, sun_path) + strlen(name);
	if (connect(fd, (struct sockaddr *)&sun, len) < 0) {
		rval = -4;
		do_unlink = 1;
		goto errout;
	}
	return(fd);

errout:
	err = errno;
	close(fd);
	if (do_unlink)
		unlink(un.sun_path);
	errno = err;
	return(rval);
}


// end cliconn

// bufargs

#define	MAXARGC		50	/* max number of arguments in buf */
#define	WHITE	" \t\n"	/* white space for tokenizing arguments */

/*
 * buf[] contains white-space-separated arguments.  We convert it to an
 * argv-style array of pointers, and call the user's function (optfunc)
 * to process the array.  We return -1 if there's a problem parsing buf,
 * else we return whatever optfunc() returns.  Note that user's buf[]
 * array is modified (nulls placed after each token).
 */
int
buf_args(char *buf, int (*optfunc)(int, char **))
{
	char	*ptr, *argv[MAXARGC];
	int		argc;

	if (strtok(buf, WHITE) == NULL)		/* an argv[0] is required */
		return(-1);
	argv[argc = 0] = buf;
	while ((ptr = strtok(NULL, WHITE)) != NULL) {
		if (++argc >= MAXARGC-1)	/* -1 for room for NULL at end */
			return(-1);
		argv[argc] = ptr;
	}
	argv[++argc] = NULL;

	/*
	 * Since argv[] pointers point into the user's buf[],
	 * user's function can just copy the pointers, even
	 * though argv[] array will disappear on return.
	 */
	return((*optfunc)(argc, argv));
}

// end bufargs


// ttymodes

static struct termios		save_termios;
static int					ttysavefd = -1;
static enum { RESET, RAW, CBREAK }	ttystate = RESET;

int
tty_cbreak(int fd)	/* put terminal into a cbreak mode */
{
	int				err;
	struct termios	buf;

	if (ttystate != RESET) {
		errno = EINVAL;
		return(-1);
	}
	if (tcgetattr(fd, &buf) < 0)
		return(-1);
	save_termios = buf;	/* structure copy */

	/*
	 * Echo off, canonical mode off.
	 */
	buf.c_lflag &= ~(ECHO | ICANON);

	/*
	 * Case B: 1 byte at a time, no timer.
	 */
	buf.c_cc[VMIN] = 1;
	buf.c_cc[VTIME] = 0;
	if (tcsetattr(fd, TCSAFLUSH, &buf) < 0)
		return(-1);

	/*
	 * Verify that the changes stuck.  tcsetattr can return 0 on
	 * partial success.
	 */
	if (tcgetattr(fd, &buf) < 0) {
		err = errno;
		tcsetattr(fd, TCSAFLUSH, &save_termios);
		errno = err;
		return(-1);
	}
	if ((buf.c_lflag & (ECHO | ICANON)) || buf.c_cc[VMIN] != 1 ||
	  buf.c_cc[VTIME] != 0) {
		/*
		 * Only some of the changes were made.  Restore the
		 * original settings.
		 */
		tcsetattr(fd, TCSAFLUSH, &save_termios);
		errno = EINVAL;
		return(-1);
	}

	ttystate = CBREAK;
	ttysavefd = fd;
	return(0);
}

int
tty_raw(int fd)		/* put terminal into a raw mode */
{
	int				err;
	struct termios	buf;

	if (ttystate != RESET) {
		errno = EINVAL;
		return(-1);
	}
	if (tcgetattr(fd, &buf) < 0)
		return(-1);
	save_termios = buf;	/* structure copy */

	/*
	 * Echo off, canonical mode off, extended input
	 * processing off, signal chars off.
	 */
	buf.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);

	/*
	 * No SIGINT on BREAK, CR-to-NL off, input parity
	 * check off, don't strip 8th bit on input, output
	 * flow control off.
	 */
	buf.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);

	/*
	 * Clear size bits, parity checking off.
	 */
	buf.c_cflag &= ~(CSIZE | PARENB);

	/*
	 * Set 8 bits/char.
	 */
	buf.c_cflag |= CS8;

	/*
	 * Output processing off.
	 */
	buf.c_oflag &= ~(OPOST);

	/*
	 * Case B: 1 byte at a time, no timer.
	 */
	buf.c_cc[VMIN] = 1;
	buf.c_cc[VTIME] = 0;
	if (tcsetattr(fd, TCSAFLUSH, &buf) < 0)
		return(-1);

	/*
	 * Verify that the changes stuck.  tcsetattr can return 0 on
	 * partial success.
	 */
	if (tcgetattr(fd, &buf) < 0) {
		err = errno;
		tcsetattr(fd, TCSAFLUSH, &save_termios);
		errno = err;
		return(-1);
	}
	if ((buf.c_lflag & (ECHO | ICANON | IEXTEN | ISIG)) ||
	  (buf.c_iflag & (BRKINT | ICRNL | INPCK | ISTRIP | IXON)) ||
	  (buf.c_cflag & (CSIZE | PARENB | CS8)) != CS8 ||
	  (buf.c_oflag & OPOST) || buf.c_cc[VMIN] != 1 ||
	  buf.c_cc[VTIME] != 0) {
		/*
		 * Only some of the changes were made.  Restore the
		 * original settings.
		 */
		tcsetattr(fd, TCSAFLUSH, &save_termios);
		errno = EINVAL;
		return(-1);
	}

	ttystate = RAW;
	ttysavefd = fd;
	return(0);
}

int
tty_reset(int fd)		/* restore terminal's mode */
{
	if (ttystate == RESET)
		return(0);
	if (tcsetattr(fd, TCSAFLUSH, &save_termios) < 0)
		return(-1);
	ttystate = RESET;
	return(0);
}

void
tty_atexit(void)		/* can be set up by atexit(tty_atexit) */
{
	if (ttysavefd >= 0)
		tty_reset(ttysavefd);
}

struct termios *
tty_termios(void)		/* let caller see original tty state */
{
	return(&save_termios);
}

// end ttymodes


// ptyopen

int
ptym_open(char *pts_name, int pts_namesz)
{
	char	*ptr;
	int		fdm, err;

	if ((fdm = posix_openpt(O_RDWR)) < 0)
		return(-1);
	if (grantpt(fdm) < 0)		/* grant access to slave */
		goto errout;
	if (unlockpt(fdm) < 0)		/* clear slave's lock flag */
		goto errout;
	if ((ptr = ptsname(fdm)) == NULL)	/* get slave's name */
		goto errout;

	/*
	 * Return name of slave.  Null terminate to handle
	 * case where strlen(ptr) > pts_namesz.
	 */
	strncpy(pts_name, ptr, pts_namesz);
	pts_name[pts_namesz - 1] = '\0';
	return(fdm);			/* return fd of master */
errout:
	err = errno;
	close(fdm);
	errno = err;
	return(-1);
}

int
ptys_open(char *pts_name)
{
	int fds;
#if defined(SOLARIS)
	int err, setup;
#endif

	if ((fds = open(pts_name, O_RDWR)) < 0)
		return(-1);

#if defined(SOLARIS)
	/*
	 * Check if stream is already set up by autopush facility.
	 */
	if ((setup = ioctl(fds, I_FIND, "ldterm")) < 0)
		goto errout;

	if (setup == 0) {
		if (ioctl(fds, I_PUSH, "ptem") < 0)
			goto errout;
		if (ioctl(fds, I_PUSH, "ldterm") < 0)
			goto errout;
		if (ioctl(fds, I_PUSH, "ttcompat") < 0) {
errout:
			err = errno;
			close(fds);
			errno = err;
			return(-1);
		}
	}
#endif
	return(fds);
}

// end ptyopen

// ptyfork

pid_t
pty_fork(int *ptrfdm, char *slave_name, int slave_namesz,
		 const struct termios *slave_termios,
		 const struct winsize *slave_winsize)
{
	int		fdm, fds;
	pid_t	pid;
	char	pts_name[20];

	if ((fdm = ptym_open(pts_name, sizeof(pts_name))) < 0)
		err_sys("can't open master pty: %s, error %d", pts_name, fdm);

	if (slave_name != NULL) {
		/*
		 * Return name of slave.  Null terminate to handle case
		 * where strlen(pts_name) > slave_namesz.
		 */
		strncpy(slave_name, pts_name, slave_namesz);
		slave_name[slave_namesz - 1] = '\0';
	}

	if ((pid = fork()) < 0) {
		return(-1);
	} else if (pid == 0) {		/* child */
		if (setsid() < 0)
			err_sys("setsid error");

		/*
		 * System V acquires controlling terminal on open().
		 */
		if ((fds = ptys_open(pts_name)) < 0)
			err_sys("can't open slave pty");
		close(fdm);		/* all done with master in child */

#if	defined(BSD)
		/*
		 * TIOCSCTTY is the BSD way to acquire a controlling terminal.
		 */
		if (ioctl(fds, TIOCSCTTY, (char *)0) < 0)
			err_sys("TIOCSCTTY error");
#endif
		/*
		 * Set slave's termios and window size.
		 */
		if (slave_termios != NULL) {
			if (tcsetattr(fds, TCSANOW, slave_termios) < 0)
				err_sys("tcsetattr error on slave pty");
		}
		if (slave_winsize != NULL) {
			if (ioctl(fds, TIOCSWINSZ, slave_winsize) < 0)
				err_sys("TIOCSWINSZ error on slave pty");
		}

		/*
		 * Slave becomes stdin/stdout/stderr of child.
		 */
		if (dup2(fds, STDIN_FILENO) != STDIN_FILENO)
			err_sys("dup2 error to stdin");
		if (dup2(fds, STDOUT_FILENO) != STDOUT_FILENO)
			err_sys("dup2 error to stdout");
		if (dup2(fds, STDERR_FILENO) != STDERR_FILENO)
			err_sys("dup2 error to stderr");
		if (fds != STDIN_FILENO && fds != STDOUT_FILENO &&
		  fds != STDERR_FILENO)
			close(fds);
		return(0);		/* child returns 0 just like fork() */
	} else {					/* parent */
		*ptrfdm = fdm;	/* return fd of master */
		return(pid);	/* parent returns pid of child */
	}
}

// end ptyfork

// lockreg

int
lock_reg(int fd, int cmd, int type, off_t offset, int whence, off_t len)
{
	struct flock	lock;

	lock.l_type = type;		/* F_RDLCK, F_WRLCK, F_UNLCK */
	lock.l_start = offset;	/* byte offset, relative to l_whence */
	lock.l_whence = whence;	/* SEEK_SET, SEEK_CUR, SEEK_END */
	lock.l_len = len;		/* #bytes (0 means to EOF) */

	return(fcntl(fd, cmd, &lock));
}

#define	read_lock(fd, offset, whence, len) \
			lock_reg((fd), F_SETLK, F_RDLCK, (offset), (whence), (len))
#define	readw_lock(fd, offset, whence, len) \
			lock_reg((fd), F_SETLKW, F_RDLCK, (offset), (whence), (len))
#define	write_lock(fd, offset, whence, len) \
			lock_reg((fd), F_SETLK, F_WRLCK, (offset), (whence), (len))
#define	writew_lock(fd, offset, whence, len) \
			lock_reg((fd), F_SETLKW, F_WRLCK, (offset), (whence), (len))
#define	un_lock(fd, offset, whence, len) \
			lock_reg((fd), F_SETLK, F_UNLCK, (offset), (whence), (len))

// end lockreg

// locktest

pid_t
lock_test(int fd, int type, off_t offset, int whence, off_t len)
{
	struct flock	lock;

	lock.l_type = type;		/* F_RDLCK or F_WRLCK */
	lock.l_start = offset;	/* byte offset, relative to l_whence */
	lock.l_whence = whence;	/* SEEK_SET, SEEK_CUR, SEEK_END */
	lock.l_len = len;		/* #bytes (0 means to EOF) */

	if (fcntl(fd, F_GETLK, &lock) < 0)
		err_sys("fcntl error");

	if (lock.l_type == F_UNLCK)
		return(0);		/* false, region isn't locked by another proc */
	return(lock.l_pid);	/* true, return pid of lock owner */
}

#define	is_read_lockable(fd, offset, whence, len) \
			(lock_test((fd), F_RDLCK, (offset), (whence), (len)) == 0)
#define	is_write_lockable(fd, offset, whence, len) \
			(lock_test((fd), F_WRLCK, (offset), (whence), (len)) == 0)
// end locktest




// error

static void	err_doit(int, int, const char *, va_list);

/*
 * Nonfatal error related to a system call.
 * Print a message and return.
 */
void
err_ret(const char *fmt, ...)
{
	va_list		ap;

	va_start(ap, fmt);
	err_doit(1, errno, fmt, ap);
	va_end(ap);
}

/*
 * Fatal error related to a system call.
 * Print a message and terminate.
 */
void
err_sys(const char *fmt, ...)
{
	va_list		ap;

	va_start(ap, fmt);
	err_doit(1, errno, fmt, ap);
	va_end(ap);
	exit(1);
}

/*
 * Nonfatal error unrelated to a system call.
 * Error code passed as explict parameter.
 * Print a message and return.
 */
void
err_cont(int error, const char *fmt, ...)
{
	va_list		ap;

	va_start(ap, fmt);
	err_doit(1, error, fmt, ap);
	va_end(ap);
}

/*
 * Fatal error unrelated to a system call.
 * Error code passed as explict parameter.
 * Print a message and terminate.
 */
void
err_exit(int error, const char *fmt, ...)
{
	va_list		ap;

	va_start(ap, fmt);
	err_doit(1, error, fmt, ap);
	va_end(ap);
	exit(1);
}

/*
 * Fatal error related to a system call.
 * Print a message, dump core, and terminate.
 */
void
err_dump(const char *fmt, ...)
{
	va_list		ap;

	va_start(ap, fmt);
	err_doit(1, errno, fmt, ap);
	va_end(ap);
	abort();		/* dump core and terminate */
	exit(1);		/* shouldn't get here */
}

/*
 * Nonfatal error unrelated to a system call.
 * Print a message and return.
 */
void
err_msg(const char *fmt, ...)
{
	va_list		ap;

	va_start(ap, fmt);
	err_doit(0, 0, fmt, ap);
	va_end(ap);
}

/*
 * Fatal error unrelated to a system call.
 * Print a message and terminate.
 */
void
err_quit(const char *fmt, ...)
{
	va_list		ap;

	va_start(ap, fmt);
	err_doit(0, 0, fmt, ap);
	va_end(ap);
	exit(1);
}

/*
 * Print a message and return to caller.
 * Caller specifies "errnoflag".
 */
static void
err_doit(int errnoflag, int error, const char *fmt, va_list ap)
{
	char	buf[MAXLINE];

	vsnprintf(buf, MAXLINE-1, fmt, ap);
	if (errnoflag)
		snprintf(buf+strlen(buf), MAXLINE-strlen(buf)-1, ": %s",
		  strerror(error));
	strcat(buf, "\n");
	fflush(stdout);		/* in case stdout and stderr are the same */
	fputs(buf, stderr);
	fflush(NULL);		/* flushes all stdio output streams */
}

// end error


// errorlog


static void	log_doit(int, int, int, const char *, va_list ap);

/*
 * Caller must define and set this: nonzero if
 * interactive, zero if daemon
 */
extern int	log_to_stderr = 1;

/*
 * Initialize syslog(), if running as daemon.
 */
void
log_open(const char *ident, int option, int facility)
{
	if (log_to_stderr == 0)
		openlog(ident, option, facility);
}

/*
 * Nonfatal error related to a system call.
 * Print a message with the system's errno value and return.
 */
void
log_ret(const char *fmt, ...)
{
	va_list		ap;

	va_start(ap, fmt);
	log_doit(1, errno, LOG_ERR, fmt, ap);
	va_end(ap);
}

/*
 * Fatal error related to a system call.
 * Print a message and terminate.
 */
void
log_sys(const char *fmt, ...)
{
	va_list		ap;

	va_start(ap, fmt);
	log_doit(1, errno, LOG_ERR, fmt, ap);
	va_end(ap);
	exit(2);
}

/*
 * Nonfatal error unrelated to a system call.
 * Print a message and return.
 */
void
log_msg(const char *fmt, ...)
{
	va_list		ap;

	va_start(ap, fmt);
	log_doit(0, 0, LOG_ERR, fmt, ap);
	va_end(ap);
}

/*
 * Fatal error unrelated to a system call.
 * Print a message and terminate.
 */
void
log_quit(const char *fmt, ...)
{
	va_list		ap;

	va_start(ap, fmt);
	log_doit(0, 0, LOG_ERR, fmt, ap);
	va_end(ap);
	exit(2);
}

/*
 * Fatal error related to a system call.
 * Error number passed as an explicit parameter.
 * Print a message and terminate.
 */
void
log_exit(int error, const char *fmt, ...)
{
	va_list		ap;

	va_start(ap, fmt);
	log_doit(1, error, LOG_ERR, fmt, ap);
	va_end(ap);
	exit(2);
}

/*
 * Print a message and return to caller.
 * Caller specifies "errnoflag" and "priority".
 */
static void
log_doit(int errnoflag, int error, int priority, const char *fmt,
         va_list ap)
{
	char	buf[MAXLINE];

	vsnprintf(buf, MAXLINE-1, fmt, ap);
	if (errnoflag)
		snprintf(buf+strlen(buf), MAXLINE-strlen(buf)-1, ": %s",
		  strerror(error));
	strcat(buf, "\n");
	if (log_to_stderr) {
		fflush(stdout);
		fputs(buf, stderr);
		fflush(stderr);
	} else {
		syslog(priority, "%s", buf);
	}
}

// end errorlog


// tellwait 

static volatile sig_atomic_t sigflag; /* set nonzero by sig handler */
static sigset_t newmask, oldmask, zeromask;

static void
sig_usr(int signo)	/* one signal handler for SIGUSR1 and SIGUSR2 */
{
	sigflag = 1;
}

void
TELL_WAIT(void)
{
	if (signal(SIGUSR1, sig_usr) == SIG_ERR)
		err_sys("signal(SIGUSR1) error");
	if (signal(SIGUSR2, sig_usr) == SIG_ERR)
		err_sys("signal(SIGUSR2) error");
	sigemptyset(&zeromask);
	sigemptyset(&newmask);
	sigaddset(&newmask, SIGUSR1);
	sigaddset(&newmask, SIGUSR2);

	/* Block SIGUSR1 and SIGUSR2, and save current signal mask */
	if (sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0)
		err_sys("SIG_BLOCK error");
}

void
TELL_PARENT(pid_t pid)
{
	kill(pid, SIGUSR2);		/* tell parent we're done */
}

void
WAIT_PARENT(void)
{
	while (sigflag == 0)
		sigsuspend(&zeromask);	/* and wait for parent */
	sigflag = 0;

	/* Reset signal mask to original value */
	if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0)
		err_sys("SIG_SETMASK error");
}

void
TELL_CHILD(pid_t pid)
{
	kill(pid, SIGUSR1);			/* tell child we're done */
}

void
WAIT_CHILD(void)
{
	while (sigflag == 0)
		sigsuspend(&zeromask);	/* and wait for child */
	sigflag = 0;

	/* Reset signal mask to original value */
	if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0)
		err_sys("SIG_SETMASK error");
}

// end tellwait