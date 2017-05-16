/*
 * klog.c for Android
 *
 *  Display kernel messages like tail -f
 *
 *      Author: Danke Xie
 *      License: GPL
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/klog.h>
#include <errno.h>
#include <string.h>
#include <sys/klog.h>
 
#define app_id "klog"
 
/*
 * Commands to klogctl:
 *
 *      0 -- Close the log.  Currently a NOP.
 *      1 -- Open the log. Currently a NOP.
 *      2 -- Read from the log.
 *      3 -- Read up to the last 4k of messages in the ring buffer.
 *      4 -- Read and clear last 4k of messages in the ring buffer
 *      5 -- Clear ring buffer.
 *      6 -- Disable printk's to console
 *      7 -- Enable printk's to console
 *      8 -- Set level of messages printed to console
 *      9 -- Return number of unread characters in the log buffer
 */
 
#define LOGBUFFER_SIZE 80
 
char log_buffer[LOGBUFFER_SIZE + 1];
 
int log_line(const char *log_buffer, int len)
{
    const int fd = STDOUT_FILENO;
    int rc;
    while (len) {
        rc = write(fd, log_buffer, len);
        if (rc == -1) {
            if (errno == EINTR)
                continue;
            else
                return -1;
        }
        len -= rc;
        log_buffer+=rc;
        fsync(fd);
    }
    return 0;
}
 
int read_log_line()
{
    int rdcnt;
    memset(log_buffer, '\0', sizeof(log_buffer));
    if ( (rdcnt = klogctl(2, log_buffer, LOGBUFFER_SIZE)) < 0 )
    {
        if ( errno == EINTR )
            return 0;
        perror(app_id);
        return -1;
    }
    else {
        return log_line(log_buffer, rdcnt);
    }
}
 
int klog_main(int argc, char *argv[])
{
 
    if (argc != 1)
    {
        // Command line usage
        printf("Usage: %s\n", app_id);
        exit(-1);
    }
 
    while (!read_log_line());
 
    return 0;
}
