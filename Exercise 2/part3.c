#include "buffered_open.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>


void add_pos_to_read_buff(buffered_file_t *bf, int count) {
    if (bf->read_buffer_pos + count < bf->read_buffer_size) {
        bf->read_buffer_pos += count;
    }
    else {
        memset(bf->read_buffer, 0, bf->read_buffer_size);
        bf->read_buffer_pos = 0;
    }
}

buffered_file_t *buffered_open(const char *pathname, int flags, ...) {
    buffered_file_t *bf = malloc(sizeof(buffered_file_t));
    if (!bf) {
        free(bf);
        return NULL;
    }
    char *read_buffer = malloc(sizeof(char) * BUFFER_SIZE);
    if (!read_buffer) {
        free(bf);
        return NULL;
    }
    char *write_buffer = malloc(sizeof(char) * BUFFER_SIZE);
    if (!write_buffer) {
        free(read_buffer);
        free(bf);
        return NULL;
    }
    bf->read_buffer = read_buffer;
    bf->write_buffer = write_buffer;
    bf->read_buffer_size = BUFFER_SIZE;
    bf->write_buffer_size = BUFFER_SIZE;
    bf->write_buffer_pos = 0;
    bf->read_buffer_pos = 0;
    // Check if flags contain O_PREAPPEND
    if (flags & O_PREAPPEND) {
        bf->preappend = 1;
        flags &= ~O_PREAPPEND;
        // Add append flag to the flags
        flags |= O_APPEND;
    } else {
        bf->preappend = 0;
    }
    bf->flags = flags;
    mode_t mode = 0;
    if (flags & O_CREAT) {
        va_list args;
        va_start(args, flags);
        mode = va_arg(args, mode_t);
        va_end(args);

        bf->fd = open(pathname, flags, mode);
    } else {
        bf->fd = open(pathname, flags);
    }
    if (bf->fd == -1) {
        free(bf->read_buffer);
        free(bf->write_buffer);
        free(bf);
        return NULL;
    }
    return bf;
}

ssize_t buffered_write(buffered_file_t *bf, const void *buf, size_t count){
    /* if O_PREAPPEND write new data in the beggging.
   if not enough space in the buffer, write the buffer to the file and then
    write to the buffer.
    */
   if (bf->preappend) {
        if (bf->write_buffer_pos + count < bf->write_buffer_size) {
            // Enough space in the buffer
            // Move the existing data to the right
            memmove(bf->write_buffer + count, bf->write_buffer, bf->write_buffer_pos);
            // Write the new data to the buffer
            memcpy(bf->write_buffer, buf, count);
            bf->write_buffer_pos += count;
            
            return count;
        }
        else {
            // not enough space in the buffer.
            // write to the remaining buffer.
            int remaining = bf->write_buffer_size - bf->write_buffer_pos;
            memmove(bf->write_buffer + remaining, bf->write_buffer, bf->write_buffer_pos);
            // Write the new data to the buffer
            int new_buff_remaining = count - remaining;
            memcpy(bf->write_buffer, buf + new_buff_remaining, remaining);
            bf->write_buffer_pos += remaining; // should be size of buffer
            // flush the buffer to the file
            if (buffered_flush(bf) == -1) {
                perror("buffered_flush");
            }
            else {
                // Write the remaining data to the buffer
                
                count -= remaining;
                buffered_write(bf, buf, count);
            }

        }
   }
   else {
        // Check if position + count is smaller than buffer size
        if (bf->write_buffer_pos + count < bf->write_buffer_size) {
            // Append to buffer
            memcpy(bf->write_buffer + bf->write_buffer_pos, buf, count);
            bf->write_buffer_pos += count;
            return count;
        }
        else {
            // not enough space in the buffer.
            // write to the remaining buffer.
            size_t remaining = bf->write_buffer_size - bf->write_buffer_pos;
            memcpy(bf->write_buffer + bf->write_buffer_pos, buf, remaining);
            bf->write_buffer_pos += remaining; // should be size of buffer
            // flush the buffer to the file
            if (buffered_flush(bf) == -1) {
                perror("buffered_flush");
            }
            else {
                // Write the remaining data to the buffer
                buf += remaining;
                count -= remaining;
                if (count == 0) {
                    return remaining;
                }
                buffered_write(bf, buf, count);
            }
        }
   }
}

int buffered_flush(buffered_file_t *bf) {
    if (bf->preappend) {
        // Get file size
        struct stat st;
        if (fstat(bf->fd, &st) == -1) {
            perror("Error getting file size");
        }
        off_t file_size = st.st_size;
        // Read the file to the buffer
        char *file_buffer = malloc(sizeof(char)*file_size);
        if (!file_buffer) {
            perror("Error allocating memory for file buffer");
            return -1;
        }
         // Reset file pointer to the beginning
        if (lseek(bf->fd, 0, SEEK_SET) == -1) {
            perror("Error seeking to start of file");
            free(file_buffer);
            return -1;
        }
        ssize_t bytes_read = read(bf->fd, file_buffer, file_size);
        if (bytes_read == -1) {
            perror("Error reading file to buffer");
            return -1;
        }
        // Clear the file
        if (ftruncate(bf->fd, 0) == -1) {
            perror("Error truncating file");
            return -1;
        }
        // Write the buffer to the file
        int bytes_written_buffer = write(bf->fd, bf->write_buffer, bf->write_buffer_pos);
        if (bytes_written_buffer == -1) {
            perror("Error writing buffer to file");
            return -1;
        }
        // Reset buffer position and clear the buffer
        bf->write_buffer_pos = 0;
        memset(bf->write_buffer, 0, bf->write_buffer_size);
        // Append the file buffer to the file
        int bytes_written_append = write(bf->fd, file_buffer, file_size);
        if (bytes_written_append == -1) {
            perror("Error writing file buffer to file");
            return -1;
        }
        // set file off set to the pos from the begging
        if (lseek(bf->fd, bytes_written_buffer, SEEK_SET) == -1) {
            perror("error setting file offset");
            free(file_buffer);
            return -1;
        }
        // Free the file buffer
        free(file_buffer);
        return bytes_written_buffer;

    }else {
    // Write the buffer to the file
    int bytes_written = write(bf->fd, bf->write_buffer, bf->write_buffer_pos);
    if (bytes_written == -1) {
        return -1;
    }
    else {
        // Successfull write, reset buffer position and clear the buffer
        bf->write_buffer_pos = 0;
        memset(bf->write_buffer, 0, bf->write_buffer_size);
        add_pos_to_read_buff(bf, bytes_written);
        return bytes_written;
    }
}
}
ssize_t buffered_read(buffered_file_t *bf, void *buf, size_t count) {
    if (bf->write_buffer_pos > 0) {
        // There is info in write buffer, flush it into file
        buffered_flush(bf);
    }
    ssize_t bytes_read = 0;
    // If read buffer is empty, read from file
    if (bf->read_buffer_pos == 0) {
        bytes_read = read(bf->fd, bf->read_buffer, bf->read_buffer_size);
        if (bytes_read == -1) {
            perror("Error reading from file");
            return -1;
        }
        if (bytes_read == 0) {
            return 0;
        }
        if (bytes_read < bf->read_buffer_size) {
        // No more data to read from file
        // Copy the data from the buffer to the output buffer
        memcpy(buf,bf->read_buffer, bytes_read);
        // File off set is alrdy at the position
        return bytes_read;
    
        }
        
    }
    
    else {// buffer already full with prevoius read
    // Set bytes_read to size of buffer - read_buffer_pos
         bytes_read = bf->read_buffer_size - bf->read_buffer_pos;}
    // Check if count is smaller than bytes read
    if (count <= bytes_read) {
        // Copy count bytes from read_buffer to buf
        memcpy(buf, bf->read_buffer + bf->read_buffer_pos, count);
        // Set read_buffer_pos to count
        bf->read_buffer_pos += count;
        // Set the file offset to the position in the buffer
        int gap = bytes_read - count;
        // Decrease the file offset by gap bytes
        off_t new_offset = lseek(bf->fd, (-1)*gap, SEEK_CUR);
        if (new_offset == (off_t)-1) {
            perror("lseek error");
        }
        return count;
    }       
    else { //count > bytes There are more bytes to read than read_buffer can hold
        // Copy bytes_read bytes from read_buffer to buf
        memcpy(buf, bf->read_buffer+bf->read_buffer_pos, bytes_read);
        if (bytes_read < bf->read_buffer_size){
        // Move file offset by bytes_read
        off_t new_offset = lseek(bf->fd, bytes_read, SEEK_CUR);
        if (new_offset == (off_t)-1) {
            perror("lseek error");
        }
        }
        // Empty the read buffer and set position to 0
        memset(bf->read_buffer, 0, bf->read_buffer_size);
        bf->read_buffer_pos = 0;
        int remaining = count - bytes_read;
        // Read the remaining bytes from the file
        buffered_read(bf, buf+bytes_read, remaining);
        }
    }

int buffered_close(buffered_file_t *bf) {
    // Flush the buffer to the file
    if (buffered_flush(bf) == -1) {
        perror("buffered_flush");
        return -1;
    }
    // Close the file
    if (close(bf->fd) == -1) {
        perror("close");
        return -1;
    }
    // Free the buffer
    free(bf->write_buffer);
    free(bf->read_buffer);
    // Free the buffered file
    free(bf);
    return 0;

}
int main() { 
    buffered_file_t *bf = buffered_open("example.txt", O_RDWR | O_CREAT | O_PREAPPEND , 0644);
    if (!bf) {
        perror("buffered_open");
        return 1;
    }
    // int fd = open("example.txt", O_RDWR );
    // int ssize_t = write(fd,"helo",4);
    // ssize_t = write(fd, "Hel", 3);
    // ssize_t = write(fd, "righ", 4);
    char buf[8];
    int ssize_t = buffered_write(bf, "Hello", 5);
    ssize_t = buffered_read(bf, buf, 7);
    if (buffered_close(bf) == -1) {
        perror("buffered_close");
        return 1;
    }


}
