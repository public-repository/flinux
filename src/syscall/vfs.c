#include "vfs.h"
#include "err.h"
#include <fs/tty.h>
#include <log.h>

#include <Windows.h>

#define MAX_FD_COUNT	1024

static struct file *vfs_fds[MAX_FD_COUNT];

void vfs_init()
{
	vfs_fds[0] = tty_alloc(GetStdHandle(STD_INPUT_HANDLE));
	vfs_fds[1] = tty_alloc(GetStdHandle(STD_OUTPUT_HANDLE));
	vfs_fds[2] = tty_alloc(GetStdHandle(STD_ERROR_HANDLE));
}

void vfs_shutdown()
{
}

size_t sys_read(int fd, char *buf, size_t count)
{
	log_debug("read(%d, %x, %d)\n", fd, buf, count);
	struct file *f = vfs_fds[fd];
	if (!f)
		return EBADF;
	return f->op_vtable->fn_read(f, buf, count);
}

size_t sys_write(int fd, const char *buf, size_t count)
{
	log_debug("write(%d, %x, %d)\n", fd, buf, count);
	struct file *f = vfs_fds[fd];
	if (!f)
		return EBADF;
	return f->op_vtable->fn_write(f, buf, count);
}

int sys_open(const char *pathname, int flags, int mode)
{
	log_debug("open(%x: \"%s\", %d, %d)\n", pathname, pathname, flags, mode);
	return -1;
}

void stat_from_stat64(struct stat *stat, struct stat64 *stat64)
{
	stat->st_dev = stat64->st_dev;
	stat->st_ino = stat64->st_ino;
	stat->st_mode = stat64->st_mode;
	stat->st_nlink = stat64->st_nlink;
	stat->st_uid = stat64->st_uid;
	stat->st_gid = stat64->st_gid;
	stat->st_rdev = stat64->st_rdev;
	stat->st_size = stat64->st_size;
	stat->st_blksize = stat64->st_blksize;
	stat->st_blocks = stat64->st_blocks;
	stat->st_atime = stat64->st_atime;
	stat->st_atime_nsec = stat64->st_atime_nsec;
	stat->st_mtime = stat64->st_mtime;
	stat->st_mtime_nsec = stat64->st_mtime_nsec;
	stat->st_ctime = stat64->st_ctime;
	stat->st_ctime_nsec = stat64->st_ctime_nsec;
	stat->__unused1 = 0;
	stat->__unused2 = 0;
}

int sys_stat(const char *pathname, struct stat *buf)
{
	struct stat64 buf64;
	int r = sys_stat64(pathname, &buf64);
	if (r == 0)
		stat_from_stat64(buf, &buf64);
	return r;
}

int sys_lstat(const char *pathname, struct stat *buf)
{
	struct stat64 buf64;
	int r = sys_lstat64(pathname, &buf64);
	if (r == 0)
		stat_from_stat64(buf, &buf64);
	return r;
}

int sys_fstat(int fd, struct stat *buf)
{
	struct stat64 buf64;
	int r = sys_fstat64(fd, &buf64);
	if (r == 0)
		stat_from_stat64(buf, &buf64);
	return r;
}

int sys_stat64(const char *pathname, struct stat64 *buf)
{
	/* TODO */
}

int sys_lstat64(const char *pathname, struct stat64 *buf)
{
	/* TODO */
}

int sys_fstat64(int fd, struct stat64 *buf)
{
	log_debug("fstat64(%d, %x)\n", fd, buf);
	struct file *f = vfs_fds[fd];
	if (!f)
		return EBADF;
	return f->op_vtable->fn_stat(f, buf);
}

int sys_ioctl(int fd, unsigned int cmd, unsigned long arg)
{
	log_debug("ioctl(%d, %d, %x)\n", fd, cmd, arg);
	struct file *f = vfs_fds[fd];
	if (!f)
		return EBADF;
	return f->op_vtable->fn_ioctl(f, cmd, arg);
}
