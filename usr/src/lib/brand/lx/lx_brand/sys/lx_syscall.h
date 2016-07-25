/*
 * CDDL HEADER START
 *
 * The contents of this file are subject to the terms of the
 * Common Development and Distribution License (the "License").
 * You may not use this file except in compliance with the License.
 *
 * You can obtain a copy of the license at usr/src/OPENSOLARIS.LICENSE
 * or http://www.opensolaris.org/os/licensing.
 * See the License for the specific language governing permissions
 * and limitations under the License.
 *
 * When distributing Covered Code, include this CDDL HEADER in each
 * file and include the License file at usr/src/OPENSOLARIS.LICENSE.
 * If applicable, add the following below this CDDL HEADER, with the
 * fields enclosed by brackets "[]" replaced with your own identifying
 * information: Portions Copyright [yyyy] [name of copyright owner]
 *
 * CDDL HEADER END
 */

/*
 * Copyright 2009 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 */

/*
 * Copyright 2016 Joyent, Inc.
 */

#ifndef _SYS_LX_SYSCALL_H
#define	_SYS_LX_SYSCALL_H

#include <sys/lx_brand.h>

#if !defined(_ASM)

#include <sys/types.h>
#include <sys/procset.h>

#ifdef __cplusplus
extern "C" {
#endif

extern int lx_install;

extern long lx_mknodat(uintptr_t, uintptr_t, uintptr_t, uintptr_t);
extern long lx_futimesat(uintptr_t, uintptr_t, uintptr_t);
extern long lx_utimensat(uintptr_t, uintptr_t, uintptr_t, uintptr_t);
extern long lx_fstatat64(uintptr_t, uintptr_t, uintptr_t, uintptr_t);
extern long lx_unlinkat(uintptr_t, uintptr_t, uintptr_t);
extern long lx_renameat(uintptr_t, uintptr_t, uintptr_t, uintptr_t);
extern long lx_symlinkat(uintptr_t, uintptr_t, uintptr_t);
extern long lx_readlinkat(uintptr_t, uintptr_t, uintptr_t, uintptr_t);

extern long lx_stat(uintptr_t, uintptr_t);
extern long lx_fstat(uintptr_t, uintptr_t);
extern long lx_lstat(uintptr_t, uintptr_t);
extern long lx_stat64(uintptr_t, uintptr_t);
extern long lx_fstat64(uintptr_t, uintptr_t);
extern long lx_lstat64(uintptr_t, uintptr_t);
extern long lx_fcntl(uintptr_t, uintptr_t, uintptr_t);
extern long lx_fcntl64(uintptr_t, uintptr_t, uintptr_t);
extern long lx_flock(uintptr_t, uintptr_t);
extern long lx_readlink(uintptr_t, uintptr_t, uintptr_t);
extern long lx_readdir(uintptr_t, uintptr_t, uintptr_t);
extern long lx_execve(uintptr_t, uintptr_t, uintptr_t);
extern long lx_dup2(uintptr_t, uintptr_t);
extern long lx_dup3(uintptr_t, uintptr_t, uintptr_t);
extern long lx_ioctl(uintptr_t, uintptr_t, uintptr_t);
extern long lx_vhangup(void);
extern long lx_fadvise64(uintptr_t, off64_t, uintptr_t, uintptr_t);
extern long lx_fadvise64_64(uintptr_t, off64_t, off64_t, uintptr_t);

extern long lx_settimeofday(uintptr_t, uintptr_t);
extern long lx_getrusage(uintptr_t, uintptr_t);
extern long lx_mknod(uintptr_t, uintptr_t, uintptr_t);

extern long lx_getpgrp(void);
extern long lx_getpgid(uintptr_t);
extern long lx_setpgid(uintptr_t, uintptr_t);
extern long lx_getsid(uintptr_t);
extern long lx_setsid(void);

extern long lx_getuid16(void);
extern long lx_getgid16(void);
extern long lx_geteuid16(void);
extern long lx_getegid16(void);
extern long lx_geteuid(void);
extern long lx_getegid(void);
extern long lx_getresuid16(uintptr_t, uintptr_t, uintptr_t);
extern long lx_getresgid16(uintptr_t, uintptr_t, uintptr_t);
extern long lx_getresuid(uintptr_t, uintptr_t, uintptr_t);
extern long lx_getresgid(uintptr_t, uintptr_t, uintptr_t);
extern long lx_capget(uintptr_t, uintptr_t);
extern long lx_capset(uintptr_t, uintptr_t);

extern long lx_setuid16(uintptr_t);
extern long lx_setreuid16(uintptr_t, uintptr_t);
extern long lx_setregid16(uintptr_t, uintptr_t);
extern long lx_setgid16(uintptr_t);
extern long lx_setfsuid16(uintptr_t);
extern long lx_setfsgid16(uintptr_t);

extern long lx_setfsuid(uintptr_t);
extern long lx_setfsgid(uintptr_t);

extern long lx_clock_nanosleep(int, int flags, struct timespec *,
    struct timespec *);
extern long lx_adjtimex(void *);
extern long lx_timer_create(int, struct sigevent *, timer_t *);
extern long lx_timer_settime(timer_t, int, struct itimerspec *,
    struct itimerspec *);
extern long lx_timer_gettime(timer_t, struct itimerspec *);
extern long lx_timer_getoverrun(timer_t);
extern long lx_timer_delete(timer_t);

extern long lx_truncate(uintptr_t, uintptr_t);
extern long lx_ftruncate(uintptr_t, uintptr_t);
extern long lx_truncate64(uintptr_t, uintptr_t, uintptr_t);
extern long lx_ftruncate64(uintptr_t, uintptr_t, uintptr_t);

extern long lx_sysctl(uintptr_t);
extern long lx_fsync(uintptr_t);
extern long lx_fdatasync(uintptr_t);
extern long lx_unlink(uintptr_t);
extern long lx_rmdir(uintptr_t);
extern long lx_rename(uintptr_t, uintptr_t);
extern long lx_utime(uintptr_t, uintptr_t);
extern long lx_llseek(uintptr_t, uintptr_t, uintptr_t, uintptr_t, uintptr_t);
extern long lx_lseek(uintptr_t, uintptr_t, uintptr_t);
extern long lx_sysfs(uintptr_t, uintptr_t, uintptr_t);

extern long lx_uname(uintptr_t);
extern long lx_reboot(uintptr_t, uintptr_t, uintptr_t, uintptr_t);
extern long lx_getgroups16(uintptr_t, uintptr_t);
extern long lx_setgroups(uintptr_t, uintptr_t);
extern long lx_setgroups16(uintptr_t, uintptr_t);

extern long lx_query_module(uintptr_t, uintptr_t, uintptr_t, uintptr_t,
    uintptr_t);

extern long lx_times(uintptr_t);
extern long lx_setitimer(uintptr_t, uintptr_t, uintptr_t);

extern long lx_clone(uintptr_t, uintptr_t, uintptr_t, uintptr_t, uintptr_t);
extern long lx_exit(uintptr_t);
extern long lx_group_exit(uintptr_t);

extern long lx_mlock(uintptr_t, uintptr_t);
extern long lx_mlockall(uintptr_t);
extern long lx_munlock(uintptr_t, uintptr_t);
extern long lx_munlockall(void);
extern long lx_msync(uintptr_t, uintptr_t, uintptr_t);
extern long lx_madvise(uintptr_t, uintptr_t, uintptr_t);
extern long lx_mprotect(uintptr_t, uintptr_t, uintptr_t);
extern long lx_mmap(uintptr_t, uintptr_t, uintptr_t, uintptr_t,
    uintptr_t, uintptr_t);
extern long lx_mmap2(uintptr_t, uintptr_t, uintptr_t, uintptr_t,
    uintptr_t, uintptr_t);
extern long lx_remap(uintptr_t, uintptr_t, uintptr_t, uintptr_t, uintptr_t);

extern long lx_mount(uintptr_t, uintptr_t, uintptr_t, uintptr_t, uintptr_t);
extern long lx_umount(uintptr_t);
extern long lx_umount2(uintptr_t, uintptr_t);

extern long lx_statfs(uintptr_t, uintptr_t);
extern long lx_fstatfs(uintptr_t, uintptr_t);
extern long lx_statfs64(uintptr_t, uintptr_t, uintptr_t);
extern long lx_fstatfs64(uintptr_t, uintptr_t, uintptr_t);

extern long lx_sigreturn(void);
extern long lx_rt_sigreturn(void);
extern long lx_signal(uintptr_t, uintptr_t);
extern long lx_sigaction(uintptr_t, uintptr_t, uintptr_t);
extern long lx_rt_sigaction(uintptr_t, uintptr_t, uintptr_t, uintptr_t);
extern long lx_sigaltstack(uintptr_t, uintptr_t);
extern long lx_sigpending(uintptr_t);
extern long lx_rt_sigpending(uintptr_t, uintptr_t);
extern long lx_sigprocmask(uintptr_t, uintptr_t, uintptr_t);
extern long lx_rt_sigprocmask(uintptr_t, uintptr_t, uintptr_t, uintptr_t);
extern long lx_sigsuspend(uintptr_t);
extern long lx_rt_sigsuspend(uintptr_t, uintptr_t);
extern long lx_rt_sigwaitinfo(uintptr_t, uintptr_t, uintptr_t);
extern long lx_rt_sigtimedwait(uintptr_t, uintptr_t, uintptr_t, uintptr_t);
extern long lx_rt_sigqueueinfo(uintptr_t, uintptr_t, uintptr_t);
extern long lx_rt_tgsigqueueinfo(uintptr_t, uintptr_t, uintptr_t, uintptr_t);

extern long lx_sync(void);

extern long lx_futex(uintptr_t, uintptr_t, uintptr_t, uintptr_t, uintptr_t,
    uintptr_t);

extern long lx_tkill(uintptr_t, uintptr_t, uintptr_t, uintptr_t, uintptr_t,
    uintptr_t);
extern long lx_tgkill(uintptr_t, uintptr_t, uintptr_t, uintptr_t, uintptr_t,
    uintptr_t);

extern long lx_sethostname(uintptr_t, uintptr_t);
extern long lx_setdomainname(uintptr_t, uintptr_t);

extern long lx_sendfile(uintptr_t, uintptr_t, uintptr_t, uintptr_t);
extern long lx_sendfile64(uintptr_t, uintptr_t, uintptr_t, uintptr_t);

extern long lx_fork(void);
extern long lx_vfork(void);
extern long lx_exec(uintptr_t, uintptr_t, uintptr_t);

extern long lx_getpriority(uintptr_t, uintptr_t);
extern long lx_setpriority(uintptr_t, uintptr_t, uintptr_t);

extern long lx_ptrace(uintptr_t, uintptr_t, uintptr_t, uintptr_t);

extern long lx_sched_getaffinity(uintptr_t, uintptr_t, uintptr_t);
extern long lx_sched_setaffinity(uintptr_t, uintptr_t, uintptr_t);
extern long lx_sched_getparam(uintptr_t, uintptr_t);
extern long lx_sched_setparam(uintptr_t, uintptr_t);
extern long lx_sched_rr_get_interval(uintptr_t pid, uintptr_t);
extern long lx_sched_getscheduler(uintptr_t);
extern long lx_sched_setscheduler(uintptr_t, uintptr_t, uintptr_t);
extern long lx_sched_get_priority_min(uintptr_t);
extern long lx_sched_get_priority_max(uintptr_t);

extern long lx_xattr2(uintptr_t, uintptr_t);
extern long lx_xattr3(uintptr_t, uintptr_t, uintptr_t);
extern long lx_xattr4(uintptr_t, uintptr_t, uintptr_t, uintptr_t);

extern long lx_keyctl(void);

extern long lx_ipc(uintptr_t, uintptr_t, uintptr_t, uintptr_t, uintptr_t);
extern long lx_msgget(key_t, int);
extern long lx_msgsnd(int, void *, size_t, int);
extern long lx_msgrcv(int, void *, size_t, long, int);
extern long lx_msgctl(int, int, void *);
extern long lx_semget(key_t, int, int);
extern long lx_semop(int, void *, size_t);
extern long lx_semtimedop(int, void *, size_t, struct timespec *);
extern long lx_semctl(int, int, int, void *);
extern long lx_shmget(key_t, size_t, int);
extern long lx_shmat(int, void *, int);
extern long lx_shmctl(int, int, void *);

extern long lx_alarm(unsigned int);
extern long lx_close(int);
extern long lx_chdir(const char *);
extern long lx_chroot(const char *);
extern long lx_creat(const char *, mode_t);
extern long lx_dup(int);
extern long lx_eventfd(unsigned int);
extern long lx_eventfd2(unsigned int, int);
extern long lx_fchdir(int);
extern long lx_getgid(void);
extern long lx_getgroups(int, gid_t *);
extern long lx_getitimer(int, struct itimerval *);
extern long lx_getuid(void);
extern long lx_inotify_add_watch(int, const char *, uint32_t);
extern long lx_inotify_init(void);
extern long lx_inotify_init1(int);
extern long lx_inotify_rm_watch(int, int);
extern long lx_mincore(caddr_t, size_t, char *);
extern long lx_munmap(void *, size_t);
extern long lx_nice(int);
extern long lx_pause(void);
extern long lx_setgid(gid_t);
extern long lx_setuid(uid_t);
extern long lx_setregid(gid_t, gid_t);
extern long lx_setreuid(uid_t, uid_t);
extern long lx_shmdt(char *);
extern long lx_signalfd(int, uintptr_t, size_t);
extern long lx_signalfd4(int, uintptr_t, size_t, int);
extern long lx_stime(const time_t *);
extern long lx_symlink(const char *, const char *);
extern long lx_syslog(int, char *, int);
extern long lx_timerfd_create(int, int);
extern long lx_timerfd_settime(int, int,
    const struct itimerspec *, struct itimerspec *);
extern long lx_timerfd_gettime(int, struct itimerspec *);
extern long lx_umask(mode_t);
extern long lx_utimes(const char *, const struct timeval *);

#endif	/* !defined(_ASM) */


#if defined(_LP64)
#define	LX_SYS_clone		56
#define	LX_SYS_vfork		58
#else
#define	LX_SYS_clone		120
#define	LX_SYS_vfork		190
#endif

#ifdef	__cplusplus
}
#endif

#endif	/* _SYS_LX_SYSCALL_H */
