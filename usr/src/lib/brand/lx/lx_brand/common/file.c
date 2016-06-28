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
 * Copyright 2016 Joyent, Inc.
 */

#include <sys/fstyp.h>
#include <sys/fsid.h>

#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/vnode.h>
#include <fcntl.h>
#include <string.h>
#include <utime.h>
#include <atomic.h>
#include <sys/syscall.h>

#include <sys/lx_syscall.h>
#include <sys/lx_types.h>
#include <sys/lx_debug.h>
#include <sys/lx_misc.h>
#include <sys/lx_fcntl.h>

#define	LX_UTIME_NOW	((1l << 30) - 1l)
#define	LX_UTIME_OMIT	((1l << 30) - 2l)

static int
install_checkpath(uintptr_t p1)
{
	int saved_errno = errno;
	char path[MAXPATHLEN];

	/*
	 * The "dev" RPM package wants to modify /dev/pts, but /dev/pts is a
	 * lofs mounted copy of /native/dev/pts, so that won't work.
	 *
	 * Instead, if we're trying to modify /dev/pts from install mode, just
	 * act as if it succeded.
	 */
	if (uucopystr((void *)p1, path, MAXPATHLEN) == -1)
		return (-errno);

	if (strcmp(path, "/dev/pts") == 0)
		return (0);

	errno = saved_errno;
	return (-errno);
}

/*
 * Convert linux LX_AT_* flags to solaris AT_* flags but skip verifying allowed
 * flags have been passed. This also allows EACCESS/REMOVEDIR to be translated
 * correctly since on linux they have the same value.
 *
 * Some code can actually pass in other bits in the flag. We may have to simply
 * ignore these, as indicated by the enforce parameter. See lx_fchmodat for
 * another example of this type of behavior.
 */
static int
ltos_at_flag(int lflag, int allow, boolean_t enforce)
{
	int sflag = 0;

	if ((lflag & LX_AT_EACCESS) && (allow & AT_EACCESS)) {
		lflag &= ~LX_AT_EACCESS;
		sflag |= AT_EACCESS;
	}

	if ((lflag & LX_AT_REMOVEDIR) && (allow & AT_REMOVEDIR)) {
		lflag &= ~LX_AT_REMOVEDIR;
		sflag |= AT_REMOVEDIR;
	}

	if ((lflag & LX_AT_SYMLINK_NOFOLLOW) && (allow & AT_SYMLINK_NOFOLLOW)) {
		lflag &= ~LX_AT_SYMLINK_NOFOLLOW;
		sflag |= AT_SYMLINK_NOFOLLOW;
	}

	/* right now solaris doesn't have a _FOLLOW flag, so use a fake one */
	if ((lflag & LX_AT_SYMLINK_FOLLOW) && (allow & LX_AT_SYMLINK_FOLLOW)) {
		lflag &= ~LX_AT_SYMLINK_FOLLOW;
		sflag |= LX_AT_SYMLINK_FOLLOW;
	}

	/* If lflag is not zero than some flags did not hit the above code. */
	if (enforce && lflag)
		return (-EINVAL);

	return (sflag);
}


/*
 * Miscellaneous file-related system calls.
 */

/*
 * On Linux, an unlink of a directory returns EISDIR, not EPERM.
 */
long
lx_unlink(uintptr_t p)
{
	char *pathname = (char *)p;
	struct stat64 statbuf;

	if ((lstat64(pathname, &statbuf) == 0) && S_ISDIR(statbuf.st_mode))
		return (-EISDIR);

	/*
	 * Some versions of the zone's /dev/log setup code (e.g. syslog-ng) get
	 * cranky if they can't cleanup /dev/log so we lie and tell them they
	 * succeeded.
	 */
	if (pathname != NULL) {
		char p[MAXPATHLEN];

		if (uucopystr((void *)pathname, p, sizeof (p)) < 0)
			return (-errno);
		if (strcmp(p, "/dev/log") == 0)
			return (0);
	}

	return (unlink(pathname) ? -errno : 0);
}

long
lx_unlinkat(uintptr_t ext1, uintptr_t p1, uintptr_t p2)
{
	int atfd = (int)ext1;
	char *pathname = (char *)p1;
	int flag = (int)p2;
	struct stat64 statbuf;

	if (atfd == LX_AT_FDCWD)
		atfd = AT_FDCWD;

	flag = ltos_at_flag(flag, AT_REMOVEDIR, B_TRUE);
	if (flag < 0)
		return (-EINVAL);

	if (!(flag & AT_REMOVEDIR)) {
		/* Behave like unlink() */
		if ((fstatat64(atfd, pathname, &statbuf, AT_SYMLINK_NOFOLLOW) ==
		    0) && S_ISDIR(statbuf.st_mode))
			return (-EISDIR);

		if (pathname != NULL) {
			char p[MAXPATHLEN];

			if (uucopystr((void *)pathname, p, sizeof (p)) < 0)
				return (-errno);
			if (strcmp(p, "/dev/log") == 0)
				return (0);
		}
	}

	return (unlinkat(atfd, pathname, flag) ? -errno : 0);
}

/*
 * fsync() and fdatasync() - On Illumos, these calls translate into a common
 * fdsync() syscall with a different parameter. fsync is handled in the
 * fsync wrapper.
 */
long
lx_fsync(uintptr_t fd)
{
	int fildes = (int)fd;
	struct stat64 statbuf;

	if ((fstat64(fildes, &statbuf) == 0) &&
	    (S_ISCHR(statbuf.st_mode) || S_ISFIFO(statbuf.st_mode)))
		return (-EINVAL);

	return (fsync((int)fd) ? -errno : 0);
}

long
lx_fdatasync(uintptr_t fd)
{
	int fildes = (int)fd;
	struct stat64 statbuf;

	if ((fstat64(fildes, &statbuf) == 0) &&
	    (S_ISCHR(statbuf.st_mode) || S_ISFIFO(statbuf.st_mode)))
		return (-EINVAL);

	return (fdatasync((int)fd) ? -errno : 0);
}

long
lx_utime(uintptr_t p1, uintptr_t p2)
{
	int ret;

	ret = utime((const char *)p1, (const struct utimbuf *)p2);

	if (ret < 0) {
		/*
		 * If utime() failed and we're in install mode, return success
		 * if the the reason we failed was because the source file
		 * didn't actually exist or if we're trying to modify /dev/pts.
		 */
		if ((lx_install != 0) &&
		    ((errno == ENOENT) || (install_checkpath(p1) == 0)))
			return (0);

		return (-errno);
	}

	return (0);
}

#if defined(_ILP32)
/*
 * llseek() - The Linux implementation takes an additional parameter, which is
 * the resulting position in the file.
 */
long
lx_llseek(uintptr_t p1, uintptr_t p2, uintptr_t p3, uintptr_t p4,
    uintptr_t p5)
{
	offset_t ret;
	offset_t *res = (offset_t *)p4;

	/* SEEK_DATA and SEEK_HOLE are only valid in Solaris */
	if ((int)p5 > SEEK_END)
		return (-EINVAL);

	if ((ret = llseek((int)p1, LX_32TO64(p3, p2), p5)) < 0)
		return (-errno);

	*res = ret;
	return (0);
}
#endif

/*
 * seek() - For 32-bit lx, when the resultant file offset cannot be represented
 * in 32 bits, Linux performs the seek but Illumos doesn't, though both set
 * EOVERFLOW.  We call llseek() and then check to see if we need to return
 * EOVERFLOW.
 */
long
lx_lseek(uintptr_t p1, uintptr_t p2, uintptr_t p3)
{
	offset_t offset = (offset_t)(off_t)(p2);	/* sign extend */
	offset_t ret;
#if defined(_ILP32)
	off_t ret32;
#endif

	/* SEEK_DATA and SEEK_HOLE are only valid in Illumos */
	if ((int)p3 > SEEK_END)
		return (-EINVAL);

	if ((ret = llseek((int)p1, offset, p3)) < 0)
		return (-errno);

#if defined(_LP64)
	return (ret);
#else
	ret32 = (off_t)ret;
	if ((offset_t)ret32 == ret)
		return (ret32);
	else
		return (-EOVERFLOW);
#endif
}

/*
 * Neither Illumos nor Linux actually returns anything to the caller, but glibc
 * expects to see SOME value returned, so placate it and return 0.
 */
long
lx_sync(void)
{
	sync();
	return (0);
}

long
lx_rmdir(uintptr_t p1)
{
	int r;
	char *nm = (char *)p1;

	r = rmdir(nm);
	if (r < 0) {
		int terr = errno;

		/*
		 * On both Illumos and Linux rmdir returns EINVAL if the last
		 * component of the path is '.', but on Illumos we also return
		 * this errno if we're trying to remove the CWD. Unfortunately,
		 * at least the LTP test suite assumes that it can rmdir the
		 * CWD, so we need handle this. We try to get out of the
		 * directory we're trying to remove.
		 */
		if (terr == EINVAL) {
			int l;

			l = strlen(nm);
			if (l >= 2 && !(nm[l - 2] == '/' && nm[l - 1] == '.')) {
				if (chdir("..") == 0 && rmdir(nm) == 0) {
					return (0);
				}
			}
		}

		return ((terr == EEXIST) ? -ENOTEMPTY : -terr);
	}
	return (0);
}

/*
 * Exactly the same as Illumos' sysfs(2), except Linux numbers their fs indices
 * starting at 0, and Illumos starts at 1.
 */
long
lx_sysfs(uintptr_t p1, uintptr_t p2, uintptr_t p3)
{
	int option = (int)p1;
	int res;

	/*
	 * Linux actually doesn't have #defines for these; their sysfs(2)
	 * man page literally defines the "option" field as being 1, 2 or 3,
	 * corresponding to Solaris' GETFSIND, GETFSTYP and GETNFSTYP,
	 * respectively.
	 */
	switch (option) {
		case 1:
			if ((res = sysfs(GETFSIND, (const char *)p2)) < 0)
				return (-errno);

			return (res - 1);

		case 2:
			if ((res = sysfs(GETFSTYP, (int)p2 + 1,
			    (char *)p3)) < 0)
				return (-errno);

			return (0);

		case 3:
			if ((res = sysfs(GETNFSTYP)) < 0)
				return (-errno);

			return (res);

		default:
			break;
	}

	return (-EINVAL);
}

long
lx_futimesat(uintptr_t p1, uintptr_t p2, uintptr_t p3)
{
	int atfd = (int)p1;
	char *path = (char *)p2;
	struct timeval *times = (struct timeval *)p3;

	if (atfd == LX_AT_FDCWD)
		atfd = AT_FDCWD;

	return (futimesat(atfd, path, times) ? -errno : 0);
}

/*
 * From the utimensat man page:
 * On Linux, futimens() is a library function implemented on top of the
 * utimensat() system call. To support this, the Linux utimensat() system
 * call implements a nonstandard feature: if pathname is NULL, then the
 * call modifies the timestamps of the file referred to by the file
 * descriptor dirfd (which may refer to any type of file). Using this
 * feature, the call futimens(fd, times) is implemented as:
 *
 *     utimensat(fd, NULL, times, 0);
 *
 * Some of the returns fail here. Linux allows the time to be modified if:
 *
 *   the caller must have write access to the file
 * or
 *   the caller's effective user ID must match the owner of the file
 * or
 *   the caller must have appropriate privileges
 *
 * We behave differently. We fail with EPERM if:
 *
 *   the calling process's euid has write access to the file but does not match
 *   the owner of the file and the calling process does not have the
 *   appropriate privileges
 *
 * This causes some of the LTP utimensat tests to fail because they expect an
 * unprivileged process can update the time on a file it can write but does not
 * own. There are also other LTP failures when the test uses attributes
 * (e.g. chattr a+) and expects a failure, but we succeed.
 */
long
lx_utimensat(uintptr_t p1, uintptr_t p2, uintptr_t p3, uintptr_t p4)
{
	int fd = (int)p1;
	const char *path = (const char *)p2;
	const timespec_t *times = (const timespec_t *)p3;
	timespec_t ts[2];
	int flag = (int)p4;

	if (times != NULL) {
		if (uucopy((void *)p3, ts, sizeof (ts)) == -1)
			return (-errno);

		if (ts[0].tv_nsec == LX_UTIME_NOW)
			ts[0].tv_nsec = UTIME_NOW;
		if (ts[1].tv_nsec == LX_UTIME_NOW)
			ts[1].tv_nsec = UTIME_NOW;

		if (ts[0].tv_nsec == LX_UTIME_OMIT)
			ts[0].tv_nsec = UTIME_OMIT;
		if (ts[1].tv_nsec == LX_UTIME_OMIT)
			ts[1].tv_nsec = UTIME_OMIT;

		times = (const timespec_t *)ts;
	}

	if (flag == LX_AT_SYMLINK_NOFOLLOW)
		flag = AT_SYMLINK_NOFOLLOW;

	if (fd == LX_AT_FDCWD)
		fd = AT_FDCWD;

	if (path == NULL) {
		return (futimens(fd, times) ? -errno : 0);
	} else {
		return (utimensat(fd, path, times, flag) ? -errno : 0);
	}
}

/*
 * Constructs an absolute path string in buf from the path of fd and the
 * relative path string pointed to by "p1". This is required for emulating
 * *at() system calls.
 * Example:
 *    If the path of fd is "/foo/bar" and path is "etc" the string returned is
 *    "/foo/bar/etc", if the fd is a file fd then it fails with ENOTDIR.
 *    If path is absolute then no modifcations are made to it when copied.
 */
static int
getpathat(int fd, uintptr_t p1, char *outbuf, size_t outbuf_size)
{
	char pathbuf[MAXPATHLEN];
	char fdpathbuf[MAXPATHLEN];
	char *fdpath;
	struct stat64 statbuf;

	if (uucopystr((void *)p1, pathbuf, MAXPATHLEN) == -1)
		return (-errno);

	/* If the path is absolute then we can early out */
	if ((pathbuf[0] == '/') || (fd == LX_AT_FDCWD)) {
		(void) strlcpy(outbuf, pathbuf, outbuf_size);
		return (0);
	}

	fdpath = lx_fd_to_path(fd, fdpathbuf, sizeof (fdpathbuf));
	if (fdpath == NULL)
		return (-EBADF);

	if ((fstat64(fd, &statbuf) < 0))
		return (-EBADF);

	if (!S_ISDIR(statbuf.st_mode))
		return (-ENOTDIR);

	if (snprintf(outbuf, outbuf_size, "%s/%s", fdpath, pathbuf) >
	    (outbuf_size-1))
		return (-ENAMETOOLONG);

	return (0);
}

long
lx_mknodat(uintptr_t ext1, uintptr_t p1, uintptr_t p2, uintptr_t p3)
{
	int atfd = (int)ext1;
	char pathbuf[MAXPATHLEN];
	int ret;

	ret = getpathat(atfd, p1, pathbuf, sizeof (pathbuf));
	if (ret < 0)
		return (ret);

	return (lx_mknod((uintptr_t)pathbuf, p2, p3));
}

long
lx_symlinkat(uintptr_t p1, uintptr_t ext1, uintptr_t p2)
{
	int atfd = (int)ext1;
	char pathbuf[MAXPATHLEN];
	int ret;

	ret = getpathat(atfd, p2, pathbuf, sizeof (pathbuf));
	if (ret < 0) {
		if (ret == -EBADF) {
			/*
			 * Try to figure out correct Linux errno. We know path
			 * is relative. Check if we have a fd for a dir which
			 * has been removed.
			 */
			if (atfd != -1 && lx_fd_to_path(atfd, pathbuf,
			    sizeof (pathbuf)) == NULL)
				ret = -ENOENT;
		}
		return (ret);
	}

	return (symlink((char *)p1, pathbuf) ? -errno : 0);
}

long
lx_readlink(uintptr_t p1, uintptr_t p2, uintptr_t p3)
{
	int ret;

	if ((size_t)p3 <= 0)
		return (-EINVAL);

	ret = readlink((char *)p1, (char *)p2, (size_t)p3);
	if (ret < 0)
		return (-errno);

	return (ret);
}

long
lx_readlinkat(uintptr_t ext1, uintptr_t p1, uintptr_t p2, uintptr_t p3)
{
	int atfd = (int)ext1;
	char pathbuf[MAXPATHLEN];
	int ret;

	if ((size_t)p3 <= 0)
		return (-EINVAL);

	ret = getpathat(atfd, p1, pathbuf, sizeof (pathbuf));
	if (ret < 0)
		return (ret);

	ret = readlink(pathbuf, (char *)p2, (size_t)p3);
	if (ret < 0)
		return (-errno);

	return (ret);
}
