// customshell.c
// Author: Eris Alihakaj

#include "customshell.h"

// Define global variables for background processes
BackgroundProcess bg_processes[MAX_BG_PROCESSES];
int bg_count = 0;
CompletedProcess completed_processes[MAX_BG_PROCESSES];
int completed_count = 0;



// Helper function to strip quotes from a string
void strip_quotes(char *str) {
    size_t len = strlen(str);

    if (len >= 2 && str[0] == '"' && str[len - 1] == '"') {
        memmove(str, str + 1, len - 1); // Shift left to remove opening quote
        str[len - 2] = '\0'; // Truncate to remove closing quote
    }
}


/*
 * Function to parse arguments and handle I/O redirection
 * This function tokenises the input string into arguments and detects special symbols
 * like '&' (background execution), '<' (input redirection), '>' (output redirection), and '>>' (append mode).
 */
void parse_arguments(char *input, char *args[], int *is_background, char **input_file, char **output_file, int *append_mode) {
    // Tokenise input into arguments using spaces and tabs as separators
    char *arg = strtok(input, SEPARATORS);
    int i = 0;
    while (arg != NULL && i < MAX_ARGS - 1) {
        strip_quotes(arg); // Strip quotes from the argument
        args[i++] = arg;
        arg = strtok(NULL, SEPARATORS);
    }
    args[i] = NULL; // Null-terminate the argument list

    // Initialize flags and file pointers
    *is_background = 0;
    *input_file = NULL;
    *output_file = NULL;
    *append_mode = 0;

    // Check for special symbols in the arguments
    for (int j = 0; args[j] != NULL; j++) {
        if (strcmp(args[j], "&") == 0) { // Background execution
            *is_background = 1;
            args[j] = NULL; // Remove '&' from the argument list
        } else if (strcmp(args[j], "<") == 0) { // Input redirection
            if (args[j + 1] != NULL) {
                *input_file = args[j + 1]; // Set the input file
                args[j] = NULL; // Remove '<' from the argument list
                j++;
            } else {
                fprintf(stderr, "Error: No input file specified for '<'.\n");
            }
        } else if (strcmp(args[j], ">") == 0) { // Output redirection (overwrite)
            if (args[j + 1] != NULL) {
                *output_file = args[j + 1]; // Set the output file
                args[j] = NULL; // Remove '>' from the argument list
                j++;
            } else {
                fprintf(stderr, "Error: No output file specified for '>'.\n");
            }
        } else if (strcmp(args[j], ">>") == 0) { // Output redirection (append)
            if (args[j + 1] != NULL) {
                *output_file = args[j + 1]; // Set the output file
                *append_mode = 1; // Enable append mode
                args[j] = NULL; // Remove '>>' from the argument list
                j++;
            } else {
                fprintf(stderr, "Error: No output file specified for '>>'.\n");
            }
        }
    }
}



int main(int argc, char **argv) {
    // Set up the for SIGCHLD to clean up terminated child processes and prevent zombies.
    struct sigaction sa;
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP; // Restart system calls and don't stop for stopped children

    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }
    
    // Set the parent environment variable for the shell itself
    set_parent_environment();

    // If a batch file is provided as an argument, process it and exit
    if (argc > 1) {
        process_batch_file(argv[1]);
        return 0;
    }

    // Buffers and variables for command processing
    char buf[MAX_BUFFER];       // Stores user input
    char *args[MAX_ARGS];       // Holds parsed arguments

    // Main loop for the shell
    while (!feof(stdin)) {
        // Check and report completed background processes
        report_completed_processes();

        // Display the shell prompt
        char *cwd = getcwd(NULL, 0);
        if (cwd) {
            printf("%s ==> ", cwd);
            free(cwd);
        } else {
            printf("==> ");
        }
        fflush(stdout);

        // Read and execute user input
        if (fgets(buf, MAX_BUFFER, stdin)) {
            // Remove the newline character from the input
            buf[strcspn(buf, "\n")] = '\0';

            // Expand environment variables in the input string
            expand_environment_variables(buf);

            // Parse arguments and handle I/O redirection
            int is_background = 0;
            char *input_file = NULL;
            char *output_file = NULL;
            int append_mode = 0;
            parse_arguments(buf, args, &is_background, &input_file, &output_file, &append_mode);

            // Execute the command
            if (args[0]) {
                if (!strcmp(args[0], "clr") || !strcmp(args[0], "cd") || !strcmp(args[0], "quit") ||
                    !strcmp(args[0], "dir") || !strcmp(args[0], "environ") || !strcmp(args[0], "echo") ||
                    !strcmp(args[0], "pause") || !strcmp(args[0], "help")) {
                    execute_internal_command(args, output_file, append_mode);
                } else {
                    execute_external_command(args, is_background, input_file, output_file, append_mode);
                }
            }
        }
    }
    return 0;  // Exit successfully
}
