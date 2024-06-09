/* C code representing shell interperator
- Accepts any number of command line arguments
- Adding arguments to PATH env
- Saving 100 commands by 100 chars matrix
- Utilizing 4 commmnads - history, cd, pwd, exit
- Loop to run up to 100 commands
- Handle errors from commands
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#define MAX_ARGS 100
char commands[100][100] = {0}; // Define command array
int command_count = 0; // Define command count


// Function to parse command line arguments and add them to the PATH
char* parse_args(int num_args, char *args[]) {
    char* base_path = getenv("PATH");
    int current_path_length = strlen(base_path);
    // Calculate total length of all arguments
    int total_length = 0;
    for (int i = 1; i < num_args; i++) {
        total_length += strlen(args[i]) + 1; // +1 for the colon or null terminator
    }
    // Step 2: Allocate memory for paths
    char *paths = (char *)malloc(total_length+current_path_length+1);
    if (paths == NULL) {
        perror("Failed to allocate memory");
        exit(1);
    }
    strcat(paths, base_path); // Copy PATH into paths
    if (current_path_length > 0) { // Not empty PATH add : to the end
            paths[current_path_length] = ':'; // Add colon after base path
        }
    // Step 3: Iterate over args and concatenate them
    for (int i = 1; i < num_args; i++) {
        strcat(paths, args[i]);
        if (i < num_args - 1) {
            strcat(paths, ":");
        }
    }
    
    
    
    return paths;
}

// Function to print command history
void print_commands() {
    // Print all commands
    for (int i = 0; i < command_count; i++) {
        printf("%s\n", commands[i]);
    }
}
// Function to add command to history commans
int add_command(char * command) {
    // Add command to commands array
    if (command_count < 100) {
        strcpy(commands[command_count], command);
        command_count++;
        
        return 1;
    }
    else {
        return -1;
    }
}

// Function to split command by space into separate components
void split_command(char* command, char* args[],int *argcc) {
    int argc = 0;
    char* token = strtok(command, " "); // split string first time
    while (token != NULL && argc < MAX_ARGS - 1) {
        args[argc++] = token; // Store each token in args array
        token = strtok(NULL, " ");
    }
    args[argc] = NULL; // Null-terminate the array
    *argcc = argc;
}
void run_command(char* command) {
    // Split command into separate components
    char* temp_args[MAX_ARGS];
    int argc = 0;
    split_command(command, temp_args, &argc);
    if (strcmp(temp_args[0], "history") == 0){
        print_commands();
        return;
    }
    else if (strcmp(temp_args[0], "cd") == 0){
        if (argc != 2) {
            perror("Usage: cd <directory>\n");
            return;
        }
        int res = chdir(temp_args[1]);
        if (res != 0) {
            perror("chdir failed");
            return;
        }
        return;
    }
    else if (strcmp(temp_args[0], "pwd") == 0){
        char cwd[4096];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("%s\n", cwd);
            return;
        }
        else {
            perror("getcwd failed");
            return;
        }
    }
    else if (strcmp(temp_args[0], "exit") == 0){
        exit(0);
    }
    
    int exe_status;
    int pid = fork();
    if (pid == -1) {
        perror("fork failed");
        exit(1);
    }
    else if (pid == 0) {
        // Child process
    exe_status = execvp(temp_args[0], temp_args); 
    if (exe_status == -1) {
        perror("execvp failed");
        exit(1);
       
        }
    }
    else {
         // Parent process
        int status;
        waitpid(pid, &status, 0);
        if (!WIFEXITED(status)) {
            printf("%s failed", temp_args[0]);
    }
    
}
}

void run_shell() {
    while (1) {
        // Print promt
        printf("$ ");
        fflush(stdout);
        // Get input from user
        char input[100];
        if (fgets(input, sizeof(input), stdin) != NULL) {
        // Remove the newline character at the end, if present
        input[strcspn(input, "\n")] = '\0';
        } else {
            perror("fgets failed");
        }
        // Add command to history
        add_command(input);
        // Run command
        run_command(input);
    }
}
int main(int argc, char *args[]) {
   
    
    // Get args from command line
    char* path = parse_args(argc, args);
    printf("PATH: %s\n", path);
    int res = setenv("PATH", path, 1);
    if (res != 0) {
        perror("Failed to set PATH");
        exit(1);
    }
    else {
        run_shell();
    }
    free(path);
    return 0;
}