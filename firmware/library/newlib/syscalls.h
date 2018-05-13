#ifndef _SYSCALLS_HPP_
#define _SYSCALLS_HPP_

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int _isatty(int fd);
int _brk(void * addr);
void * _sbrk(int increment);
ssize_t _write(int fd, const void *buf, size_t count);
int _close(int fd);
int _fstat(int fd, struct stat *buf);
int _isatty(int fd);
off_t _lseek(int fd, off_t offset, int whence);
ssize_t _read(int fd, void *buf, size_t count);

#endif
