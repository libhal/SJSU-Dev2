#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "L0-LowLevel/uart0.min.h"

#include "syscalls.h"

// sbrkr.c:(.text._sbrk_r+0xc): undefined reference to `_sbrk'
void * heap;
int _brk(void * addr)
{
    heap = addr;
}
void * _sbrk(int increment)
{
    heap += increment;
    return heap;
}
// writer.c:(.text._write_r+0x10): undefined reference to `_write'
ssize_t _write(int fd, const void *buf, size_t count)
{
    char * string_buffer = (char *)buf;
    for(int i = 0; i < count; i++)
    {
        uart0_putchar(string_buffer[i]);
    }
    return count;
}
// closer.c:(.text._close_r+0xc): undefined reference to `_close'
int _close(int fd)
{
    (void)fd;
    return 0;
}
// fstatr.c:(.text._fstat_r+0xe): undefined reference to `_fstat'
int _fstat(int fd, struct stat *buf)
{

}
// isattyr.c:(.text._isatty_r+0xc): undefined reference to `_isatty'
int _isatty(int fd)
{
    //// Remove unused warning
    (void)fd;
    return 1;
}
// lseekr.c:(.text._lseek_r+0x10): undefined reference to `_lseek'
off_t _lseek(int fd, off_t offset, int whence)
{
    (void)fd;
    static unsigned int terminal_position = 0;
    switch(whence)
    {
        case SEEK_SET:
            terminal_position = offset;
            break;
       case SEEK_CUR:
            terminal_position += offset;
            break;
       case SEEK_END:
            terminal_position = (-1) - offset;
            break;
        default:
            break;
    }
    return terminal_position;
}
// readr.c:(.text._read_r+0x10): undefined reference to `_read'
ssize_t _read(int fd, void *buf, size_t count)
{
    (void)fd;
    char * string_buffer = (char *)buf;
    for(int i = 0; i < count; i++)
    {
        string_buffer[i] = uart0_getchar('\0');
    }
    return count;
}