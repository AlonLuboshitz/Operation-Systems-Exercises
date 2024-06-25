#include "copytree.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <stdbool.h>
#include <limits.h>
#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>

#define BUF_SIZE 4096


/* Function to copy file from src to dest*/
void copy_file(const char *src, const char *dest, int copy_symlinks, int copy_permissions) {
    // 1. Check src file exists.
    int src_fd = open(src, O_RDONLY);
    if (src_fd == -1) {
        perror("open src file failed");
        return;
    }
    struct stat src_stat;
    mode_t src_mode;
    // Check permissions.
    if (copy_permissions == 1) {
       
        // Get the source file's permissions
        if (fstat(src_fd, &src_stat) == -1) {
            perror("fstat source");
            close(src_fd);
            return ;
            }
        src_mode = src_stat.st_mode;
    }
    else {
         src_mode = 0666;
    }
    // Check smybolic links.
    bool is_link = false;
    if (copy_symlinks == 1) {
        if (lstat(src, &src_stat) == -1) {
            perror("lstat source");
            close(src_fd);
            return ;
        }
        else {
            if (S_ISLNK(src_stat.st_mode)) {
                is_link = true;
            }
        }
    }
    // Copy file from src to dest with links and permmisions if given
    if (is_link) {
        char target[BUF_SIZE];
        ssize_t len = readlink(src, target, sizeof(target) - 1);
        if (len == -1) {
            perror("readlink");
            return ;
        }
        target[len] = '\0'; // Null-terminate the target path
        // Create a new symbolic link with the same target
            if (symlink(target, dest) == -1) {
                perror("symlink");
                return ;
            }
        
    }
    else {
        // copy file content
        // 2. Check dest folder exists.
        int dest_fd = open(dest, O_WRONLY | O_CREAT | O_TRUNC, src_mode );
        if (chmod(dest, src_mode) == -1) { // change mode if file alrdy exists
        perror("chmod failed");
        return ;
        }
        if (dest_fd == -1) {
            perror("open dest file failed");
            close(src_fd);
            return;
        }

        char buf[BUF_SIZE];
        ssize_t bytes_read;
        while ((bytes_read = read(src_fd, buf, BUF_SIZE)) > 0) { // Copy contect
            ssize_t bytes_written = write(dest_fd, buf, bytes_read);
            if (bytes_written == -1) {
                perror("write");
                close(src_fd);
                close(dest_fd);
                return;
            }
        }
            // Check for read error
        if (bytes_read == -1) {
            perror("read");
        }

        // Close files
        close(src_fd);
        close(dest_fd);
        
    }
    

}

int dir_creation(const char* dir_name,mode_t mode, bool is_link,char * target, int copy_permissions) {
    if (is_link) {
        if (symlink(target, dir_name) == -1) {
            perror("symlink");
            return -1;
        }
        else {
            DIR *dir = opendir(dir_name);
        if (dir == NULL) {
        perror("opendir");
        return -1;
     }
        else {
        closedir(dir);
        return 1;
        }
        }
    }
    
    
    
    // Create the directory
    if (mkdir(dir_name, mode) == -1) {
        perror("mkdir");
        return -1;
    }
    DIR *dir = opendir(dir_name);
    if (dir == NULL) {
        perror("opendir");
        return -1;
    }
    else {
        closedir(dir);
        return 1;
    }
}



void copy_directory(const char *src, const char *dest, int copy_symlinks, int copy_permissions) {
    // Valid src is a directory
    struct stat src_stat;
    if (stat(src, &src_stat) == -1) {
        perror("stat source");
        return;
    }
    if (!S_ISDIR(src_stat.st_mode)) {
        fprintf(stderr, "Source is not a directory\n");
        return;
    } 
    bool is_link = false;
    char target[BUF_SIZE];
    if (copy_symlinks == 1) {
        if (lstat(src, &src_stat) == -1) {
            perror("lstat source");
            return ;
        }
        else {
            if (S_ISLNK(src_stat.st_mode)) {
                is_link = true;
                
            ssize_t len = readlink(src, target, sizeof(target) - 1);
            if (len == -1) {
                perror("readlink");
                return ;
            }
            target[len] = '\0'; // Null-terminate the target path

            
                }
        }
    }
   
    mode_t src_mode = src_stat.st_mode;
    // create dest directory
    DIR *dest_dir_ptr = opendir(dest);
    if (dest_dir_ptr == NULL) {
        if (dir_creation(dest, src_mode, is_link,target, copy_permissions) ==-1) {
            perror("mkdir");
            return;
        }
    }
    else {
        closedir(dest_dir_ptr);
    }
    if (is_link) {
        return;
    }
    // Open the source directory
    DIR *dir = opendir(src);
    if (dir == NULL) {
        perror("opendir");
        return;
    }
    struct dirent *entry;
    
    while ((entry=readdir(dir))!= NULL) {
         if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue; // Skip current directory (.) and parent directory (..)
        }
        char dest_dir[BUF_SIZE];
        strcpy(dest_dir, dest);
        // Check if the entry is a file or directory
        char path[BUF_SIZE];
        snprintf(path, sizeof(path), "%s/%s", src, entry->d_name);

        struct stat statbuf;
        if (stat(path, &statbuf) == -1) {
            perror("stat");
            closedir(dir);
            return ;
        }
        strcat(dest_dir, "/");
        strcat(dest_dir, entry->d_name);
        if (S_ISDIR(statbuf.st_mode)) {
             // Concatenate entry name to dest path
            
            copy_directory(path, dest_dir, copy_symlinks, copy_permissions);
        } else if (S_ISREG(statbuf.st_mode)) {
            copy_file(path, dest_dir, copy_symlinks, copy_permissions);

        }   

}
    closedir(dir);
}




int main() {
    copy_directory("/home/alon/Op_sys/Operation-Systems-Exercises-1/Exercise 2/source_directory", "destination_directory", 1, 0);
    return 0;
}