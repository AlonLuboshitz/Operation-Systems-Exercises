#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>


int main() {
    int fd = open("test.txt", O_CREAT | O_RDWR, 0644);
    char buf[4096];
    ssize_t written = write(fd, "Hello, World!", 13);
    off_t offset = lseek(fd, 0, SEEK_SET);
    if (offset == -1) {
        perror("lseek");
        exit(1);
    }
    ssize_t read_b = read(fd, buf, 20);
    
    printf("Read: %s\n", buf);
}