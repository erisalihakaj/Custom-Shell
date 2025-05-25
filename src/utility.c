// utility.c
// Author: Eris Alihakaj
// Student_ID: 23305593
// I Acknowledge DCU's Academic Integrity Policy 

#include "customshell.h"

/*
 * change_dir - Changes the current working directory.
 * If no directory is provided, it prints the current directory.
 */
void change_dir(const char *target_dir) {
    char directory[MAX_BUFFER];

    if (target_dir == NULL) {
        // No argument given; print the current directory
        if (getcwd(directory, sizeof(directory)) != NULL) {
            printf("%s\n", directory);
        } else {
            perror("Error retrieving current directory");
        }
        return;
    }

    // Attempt to change to the target directory
    if (chdir(target_dir) != 0) {
        fprintf(stderr, "cd: %s: No such file or directory\n", target_dir); // Print an error if changing directory fails
        return;
    }

    // Update the PWD (Present Working Directory) environment variable
    if (getcwd(directory, sizeof(directory)) != NULL) {
        if (setenv("PWD", directory, 1) != 0) {
            perror("Failed to update PWD");
        }
    }
}



/*
 * list_environ - Prints all environment variables.
 * It also sets the SHELL variable to "customshell".
 */
void list_environ() {
    // Overwrite SHELL to explicitly say "customshell"
    if (setenv("SHELL", "customshell", 1) != 0) {
        perror("Failed to set SHELL environment variable");
    }

    char **env_ptr = environ;  // Pointer to environment variables
    
    while (*env_ptr != NULL) { // Iterate and print each environment variable
        puts(*env_ptr);
        env_ptr++;  // Move to the next environment variable
    }
}



/*
 * print_echo - Mimics the behavior of the echo command.
 * Prints all arguments passed after 'echo'.
 * If output_file is not NULL, redirects output to the specified file.
 */
void print_echo(char *args[], const char *output_file, int append_mode) {
    // Save the original stdout file descriptor
    int original_stdout = dup(STDOUT_FILENO);

    // Open the output file if specified
    int fd = -1;
    if (output_file) {
        int flags = O_WRONLY | O_CREAT;
        if (append_mode) {
            flags |= O_APPEND; // Append mode
        } else {
            flags |= O_TRUNC;  // Truncate mode
        }
        fd = open(output_file, flags, 0644); // 0644 sets file permissions (rw-r--r--)
        if (fd == -1) {
            perror("open");
            return;
        }
        dup2(fd, STDOUT_FILENO); // Redirect stdout to the file
    }

    // Print all arguments after 'echo'
    int i = 1; // Start from index 1 to skip the 'echo' command
    while (args[i] != NULL) {
        printf("%s", args[i]); // Print the argument
        if (args[i + 1] != NULL) {
            printf(" "); // Print a space between arguments
        }
        i++;
    }
    printf("\n"); // Print a newline at the end

    // Restore stdout if redirection was used
    if (fd != -1) {
        dup2(original_stdout, STDOUT_FILENO); // Restore original stdout
        close(fd); // Close the file descriptor
    }
    close(original_stdout); // Close the saved file descriptor
}



/*
 * list_dir - Lists the contents of a directory using the 'ls -al' command.
 * If no directory is specified, it lists the current directory.
 */
void list_dir(const char *cur_path) {
    char command[MAX_BUFFER];

    // Format command: 'ls -al <directory>' if a path is provided, else 'ls -al'
    if (cur_path) {
        snprintf(command, sizeof(command), "ls -al %s", cur_path);
    } else {
        snprintf(command, sizeof(command), "ls -al");
    }
    if (system(command) == -1) {
        perror("system");
    }
}



/*
 * pause_shell - Pauses execution until the user presses Enter.
 * The shell will not accept any input during this time.
 */
void pause_shell() {
    printf("Press Enter to continue...");
    while (getchar() != '\n'); // Wait until Enter is pressed
    printf("\n");
}



/*
 * process_batch_file - Reads and executes commands from a batch file.
 * Each line in the file is treated as a separate command.
 */
void process_batch_file(const char *filename) {
    FILE *file = fopen(filename, "r"); // Open the batch file for reading
    if (!file) {
        perror("Error opening batch file");
        exit(1);
    }

    char line[MAX_BUFFER];
    while (fgets(line, sizeof(line), file)) { // Read each line from the file
        // Remove trailing '\r' and '\n' characters
        line[strcspn(line, "\r\n")] = '\0';

        printf("Executing: %s\n", line);
        char *args[MAX_ARGS];
        int i = 0;

        // Tokenise the line using defined separators (whitespace, newlines, etc.)
        args[i] = strtok(line, SEPARATORS);
        while (args[i] != NULL && i < MAX_ARGS - 1) {
            i++;
            args[i] = strtok(NULL, SEPARATORS);
        }

        // Execute the corresponding function for each command
        if (args[0]) {
            if (!strcmp(args[0], "clr")) {
                system("clear");
            } else if (!strcmp(args[0], "cd")) {
                change_dir(args[1]);
            } else if (!strcmp(args[0], "quit")) {
                exit(0);
            } else if (!strcmp(args[0], "dir")) {
                list_dir(args[1]);
            } else if (!strcmp(args[0], "environ")) {
                list_environ();
            } else if (!strcmp(args[0], "echo")) {
                print_echo(args, NULL, 0);
            } else if (!strcmp(args[0], "pause")) {
                pause_shell();
            } else if (!strcmp(args[0], "help")) {
                show_help();
            }
        }
    }
    fclose(file);
}



/*
 * show_help - Displays a user manual from "manual.txt".
 * Uses the 'more' command for paginated output.
 */
void show_help() {
    char manual_path[1024];
    char exe_path[1024];

    // Get the path of the executable
    if (readlink("/proc/self/exe", exe_path, sizeof(exe_path)) == -1) {
        perror("Error getting executable path");
        return;
    }

    // Construct the path to the manual file
    snprintf(manual_path, sizeof(manual_path), "%s/../manual/manual.txt", dirname(exe_path));

    FILE *file = fopen(manual_path, "r"); // Open manual file
    if (!file) {
        perror("Error opening manual file");
        return;
    }

    char line[MAX_BUFFER];
    FILE *fp = popen("more", "w");  // Open 'more' for paginated display

    if (fp != NULL) {
        while (fgets(line, sizeof(line), file)) { // Read and write line-by-line
            fputs(line, fp);
        }
        pclose(fp);  // Close 'more'
    } else {
        // If 'more' fails, print directly to the console
        while (fgets(line, sizeof(line), file)) {
            fputs(line, stdout);
        }
    }

    fclose(file);  // Close the manual file
}



/*
 * Signal handler for SIGCHLD to clean up terminated child processes and prevent zombies.
 * This function is called when a child process terminates. It prevents zombie processes
 * by reaping them and notifying the user about their exit status.
 */
void sigchld_handler(int sig) {
    (void)sig; // Mark parameter as unused

    int status;
    pid_t pid;

    // Wait for any child process to terminate (non-blocking)
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        // Store the completed process
        if (completed_count < MAX_BG_PROCESSES) {
            completed_processes[completed_count].pid = pid;
            completed_processes[completed_count].status = status;
            completed_processes[completed_count].exited = WIFEXITED(status);
            completed_count++;
        }

        // Remove process from background process list
        for (int i = 0; i < bg_count; i++) {
            if (bg_processes[i].pid == pid) {
                bg_processes[i] = bg_processes[bg_count - 1]; // Replace with last process
                bg_count--;
                break;
            }
        }
    }
}




/*
 * Function to check and report background processes
 * This function iterates through the list of background processes and checks if any have completed.
 * If a process has completed, it is removed from the list and the user is notified.
 */
void report_completed_processes() {
    for (int i = 0; i < completed_count; i++) {
        if (completed_processes[i].exited) {
            printf("[%d]  + done       %s (exited with status %d)\n",
                   completed_processes[i].pid, completed_processes[i].command, WEXITSTATUS(completed_processes[i].status));
        } else {
            printf("[%d]  + done       %s (terminated by signal %d)\n",
                   completed_processes[i].pid, completed_processes[i].command, WTERMSIG(completed_processes[i].status));
        }
    }
    completed_count = 0; // Reset the list
}