/* Alon luboshitz 312115090
Handle files
*/

//Includes
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
//Parse args
int is_integer(const char *str) {
    if (*str == '\0' || isspace(*str)) {
        // Empty string or string with only spaces is not a valid integer
        return 0;
    }
    
    char *endptr;
    long val = strtol(str, &endptr, 10);
    
    // Check if endptr points to the end of the string, indicating a successful conversion
    // Also ensure the entire string is used in the conversion
    if (*endptr == '\0' && endptr != str) {
        return 1;  // str is a valid integer
    } else {
        return 0;  // str is not a valid integer
    }
}
int parse_args(int argc, char* argv[], int* count){
    if (argc != 5) {
    fprintf(stderr, "Usage: %s <parent_message> <child1_message> <child2_message> <count>", argv[0]);
    return -1;
    }
    // last arg is count of messages - validate its integer and bigger then 0
    else if(!is_integer(argv[4]) || atoi(argv[4]) <= 0) {
        fprintf(stderr, "Error: count must be a positive integer\n");
        return -1;
    }
    else  {
        *count = atoi(argv[4]);
    return 1;
    }
}

//Run function


void run(int count, char* parent_message, char* child1_message, char* child2_message){
    // Open output.txt file with O_CREAT and O_APPEND
    int file_fd = open("output.txt", O_CREAT | O_TRUNC | O_WRONLY, 0666);
    if (file_fd == -1) {
        perror("Error: failed to open file\n");
        return;
    }
    int len_1 = strlen(child1_message);
    int len_2 = strlen(child2_message);
    int len_3 = strlen(parent_message);
    char* child1_message_with_new_line = (char*) malloc(len_1 + 2);
    char* child2_message_with_new_line = (char*) malloc(len_2 + 2);
    char* parent_message_with_new_line = (char*) malloc(len_3 + 2);
    
    // Copy messages and append new line
    strcpy(child1_message_with_new_line, child1_message);
    strcpy(child2_message_with_new_line, child2_message);
    strcpy(parent_message_with_new_line, parent_message);
    strcat(child1_message_with_new_line, "\n");
    strcat(child2_message_with_new_line, "\n");
    strcat(parent_message_with_new_line, "\n");

    
    // Create 2 child processes
    pid_t child1 = fork();
    if (child1 == -1) {
        perror("Error: failed to fork\n");
        close(file_fd);
        return;
    }
    else if (child1 == 0) {
        // Child 1
        for (int i = 0; i < count; i++) {
            write(file_fd, child1_message_with_new_line, strlen(child1_message_with_new_line));
        }
        close(file_fd);
        return;
    }
    else {
        // Parent
        pid_t child2 = fork();
        if (child2 == -1) {
            perror("Error: failed to fork\n");
            close(file_fd);
            return;
        }
        else if (child2 == 0) {
            // Child 2
            for (int i = 0; i < count; i++) {
                write(file_fd, child2_message_with_new_line, strlen(child2_message_with_new_line));
            }
            close(file_fd);
            return;
        }
        else {
            // Parent
            // Wait for both children to finish
            if (waitpid(child1, NULL, 0) != child1) {
                perror("Error: failed to wait for child1\n");
                close(file_fd);
                return;
            }
            if (waitpid(child2, NULL, 0) != child2) {
                perror("Error: failed to wait for child2\n");
                close(file_fd);
                return;
            }
            for (int i = 0; i < count; i++) {
                write(file_fd, parent_message_with_new_line, strlen(parent_message_with_new_line));
            }
        }
    }

    // Remove last /n from file
    lseek(file_fd, -1, SEEK_END);
    ftruncate(file_fd, lseek(file_fd, 0, SEEK_CUR));
    
    // Close file
    close(file_fd);
    free(child1_message_with_new_line);
    free(child2_message_with_new_line);
    free(parent_message_with_new_line);
}



//Main function
int main(int argc, char* argv[]){
    int count = 0;
    parse_args(argc, argv, &count);
    run(count, argv[1], argv[2], argv[3]);
    return 0;
}