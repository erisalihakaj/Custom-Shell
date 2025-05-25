// execute_commands.c
// Author: Eris Alihakaj
// Student_ID: 23305593
// I Acknowledge DCU's Academic Integrity Policy 

#include "customshell.h"

// Executes an external command with optional I/O redirection and background execution.
void execute_external_command(char *args[], int is_background, const char *input_file, const char *output_file, int append_mode) {
    pid_t pid = fork(); // Create a child process

    if (pid < 0) {
        perror("fork failed"); // Fork failed
        return;
    } 
    else if (pid == 0) {
        // Child process

        // Input redirection: redirect stdin to the specified file
        if (input_file) {
            int input_fd = open(input_file, O_RDONLY); // Open input file
            if (input_fd == -1) {
                fprintf(stderr, "Error: Cannot open input file - %s\n", input_file);
                exit(EXIT_FAILURE);
            }
            if (dup2(input_fd, STDIN_FILENO) == -1) { // Redirect stdin
                perror("dup2 failed");
                close(input_fd);
                exit(EXIT_FAILURE);
            }
            close(input_fd); // Close file descriptor
        }

        // Output redirection: redirect stdout to the specified file
        if (output_file) {
            int flags = O_WRONLY | O_CREAT | (append_mode ? O_APPEND : O_TRUNC); // Append or truncate
            int output_fd = open(output_file, flags, 0644); // Open output file
            if (output_fd == -1) {
                fprintf(stderr, "Error: Cannot open output file - %s\n", output_file);
                exit(EXIT_FAILURE);
            }
            if (dup2(output_fd, STDOUT_FILENO) == -1) { // Redirect stdout
                perror("dup2 failed");
                close(output_fd);
                exit(EXIT_FAILURE);
            }
            close(output_fd); // Close file descriptor
        }

        // Set the parent environment variable to the shell's path
        char path[MAX_BUFFER];
        if (readlink("/proc/self/exe", path, sizeof(path) - 1) != -1) {
            path[sizeof(path) - 1] = '\0'; // Null-terminate
            if (setenv("parent", path, 1) != 0) { // Set environment variable
                perror("setenv failed");
                exit(EXIT_FAILURE);
            }
        }

        // Execute the command using execvp
        if (execvp(args[0], args) == -1) {
            fprintf(stderr, "Error: Command not found - %s\n", args[0]); // Command not found
            exit(EXIT_FAILURE);
        }
    } 
    else {
        // Parent process
        if (!is_background) {
            // Foreground execution: wait for the child process to complete
            int status;
            if (waitpid(pid, &status, 0) == -1) { // Wait for the child process
                perror("waitpid failed");
                return;
            }

            // Check and report exit status
            if (WIFEXITED(status)) {
                int exit_status = WEXITSTATUS(status);
                if (exit_status != 0) {
                    fprintf(stderr, "Command exited with status %d\n", exit_status);
                }
            } 
            else if (WIFSIGNALED(status)) {
                fprintf(stderr, "Command terminated by signal %d\n", WTERMSIG(status));
            }
        } 
        else {
            // Background execution: print the PID and continue
            printf("Background process started. PID: %d\n", pid);
        }
    }
}


/*
 * Execute internal shell commands and output redirection
 * This function handles built-in commands like "cd", "clr", "quit", etc.
 * It also supports output redirection to files (e.g., "dir > output.txt").
 */
void execute_internal_command(char *args[], char *output_file, int append_mode) {
    int saved_stdout = -1;
    
    // Handle output redirection if specified
    if (output_file) {
        int flags = O_WRONLY | O_CREAT | (append_mode ? O_APPEND : O_TRUNC); // Append or truncate the file
        int output_fd = open(output_file, flags, 0644); // Open the file with read/write permissions
        if (output_fd == -1) {
            perror("Error opening output file");
            return;
        }
        saved_stdout = dup(STDOUT_FILENO); // Save the current stdout
        dup2(output_fd, STDOUT_FILENO); // Redirect stdout to the file
        close(output_fd); // Close the file descriptor
    }

    // Execute the internal command based on the first argument
    if (!strcmp(args[0], "clr")) {                      // Clears the terminal
        system("clear");
    } else if (!strcmp(args[0], "cd")) {                // Change directory
        change_dir(args[1]);
    } else if (!strcmp(args[0], "quit")) {              // Exit the shell
        exit(0);
    } else if (!strcmp(args[0], "dir")) {               // List directory contents
        list_dir(args[1]);
    } else if (!strcmp(args[0], "environ")) {           // Display environment variables
        list_environ();
    } else if (!strcmp(args[0], "echo")) {              // Print text to console
        print_echo(args, output_file, append_mode);
    } else if (!strcmp(args[0], "pause")) {             // Pause execution
        pause_shell();              
    } else if (!strcmp(args[0], "help")) {              // Show the help manual
        show_help();
    }

    // Restore stdout if output was redirected
    if (output_file && saved_stdout != -1) {
        dup2(saved_stdout, STDOUT_FILENO); // Restore the original stdout
        close(saved_stdout); // Close the saved file descriptor
    }
}