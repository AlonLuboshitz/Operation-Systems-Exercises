/* Alon Luboshitz 312115090 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>
//Function to parse args
int parse_args(int argc,char* argv[],char* messages[],int* count) {
    // check if the number of arguments is correct
   
    // last argument is count
    *count = atoi(argv[argc-1]);
    for (int i= 1;i<argc-1;i++){
        messages[i-1] = argv[i];
        printf("message %d: %s\n",i,messages[i-1]);
    }
    
    return 1;
}
void write_message(const char *message, int count) {
    for (int i = 0; i < count; i++) {
        printf("%s", message);
        usleep((rand() % 100) * 1000); // Random delay between 0 and 99 milliseconds
    }
}

/*
Run function
1. init a lock file
2. Accpet pointer for array of messages.
each message assigned to diffrenet process
3. Create num processes and make them write the messages*/
void run(int num_processes, char* messages[], int count){
    // Open output2.txt file
    int output_fd = open("output2.txt", O_CREAT | O_WRONLY | O_APPEND, 0666);
    if (output_fd == -1) {
        perror("open");
        exit(1);
    }

    // Redirect stdout to the output file
    if (dup2(output_fd, STDOUT_FILENO) == -1) {
        perror("dup2");
        close(output_fd);
        exit(1);
    }
    
    // Close the original file descriptor as it's no longer needed
    close(output_fd); 
    for (int i = 0; i < num_processes; i++){
        // create child processes
        const char* message = messages[i];
        pid_t pid = fork();
        if (pid == 0){
            // Child process
             int lock_fd;
                while (1) {
                    lock_fd = open("lockfile.lock", O_CREAT | O_EXCL | O_RDWR, 0666);
                    if (lock_fd != -1) {
                        // Lock file created successfully
                        write_message(message, count);
                        // Close and remove the lock file
                        close(lock_fd);
                        if (remove("lockfile.lock") == 0) {
                            exit(0);
                        } else {
                            perror("Error removing lock file");
                            exit(1);
                        }
                    } else {
                        
                        // Lock file exists, wait for 10 milliseconds
                        usleep(10000);
                    }
                }
            }
                        


    }
    // Wait for all child processes to finish
    for (int i = 0; i < num_processes; i++){
        if(waitpid(-1, NULL, 0) == -1){
            perror("Error waiting for child process");
        }
}
}

int main(int argc, char* argv[]){
    if(argc <= 4) {
        fprintf(stderr, "Usage: %s <message1> <message2> ... <count>", argv[0]);
    }
    char* messages[argc-2];
    int count;
    parse_args(argc, argv, messages, &count);
    
    run((argc-2), messages,count);
    return 0;
}