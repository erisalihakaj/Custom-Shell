// customshell.h
// Author: Eris Alihakaj

#ifndef CUSTOMSHELL_H
#define CUSTOMSHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <libgen.h>
#include <signal.h>
#include <ctype.h>

// Constants
#define MAX_BUFFER 1024        // Max buffer size for input/strings
#define MAX_ARGS 64            // Max number of command arguments
#define SEPARATORS " \t\n"     // Token separators for parsing input
#define MAX_BG_PROCESSES 100   // Max background processes to track

// Structure to track background processes
typedef struct {
    pid_t pid;                 // Process ID of the background process
    char command[MAX_BUFFER];  // Command associated with the process
} BackgroundProcess;

// Structure to track background processes
typedef struct {
    pid_t pid;                 // Process ID of the background process
    char command[MAX_BUFFER];  // Command associated with the process
    int status;                // Exit status or signal number
    int exited;                // 1 if exited normally, 0 if terminated by signal
} CompletedProcess;

// Global variables
extern BackgroundProcess bg_processes[MAX_BG_PROCESSES];
extern int bg_count;
extern CompletedProcess completed_processes[MAX_BG_PROCESSES];
extern int completed_count;

// Function prototypes
void sigchld_handler(int sig);                                                              // Signal handler for SIGCHLD to clean up terminated child processes
void report_completed_processes();                                                          // Checks and reports background processes that have completed
void expand_environment_variables(char *input);                                             // Expands environment variables (e.g., $HOME) in the input string
void execute_internal_command(char *args[], char *output_file, int append_mode);            // Executes internal shell commands (e.g., cd, clr, quit)
void parse_arguments(char *input, char *args[], int *is_background, char **input_file, char **output_file, int *append_mode);   // Parses command-line arguments and handles I/O redirection
void show_help();                                                                           // Displays help manual with available commands and usage.
void change_dir(const char *target_dir);                                                    // Changes current directory to `target_dir` or home if none provided.
void list_environ();                                                                        // Lists all environment variables and their values.
void print_echo(char *args[], const char *output_file, int append_mode);                    // Prints text to console or redirects to a file (appends if `append_mode` is set).
void list_dir(const char *cur_path);                                                        // Lists contents of `cur_path` or current directory if `cur_path` is NULL.
void pause_shell();                                                                         // Pauses shell execution until user presses Enter.
void process_batch_file(const char *filename);                                              // Executes commands from a batch file line by line.
void execute_external_command(char *args[], int is_background, const char *input_file, const char *output_file, int append_mode);       // Runs external commands with support for background execution and I/O redirection.
int handle_io_redirection(char *args[], int *input_fd, int *output_fd, int *append_mode);   // Handles I/O redirection (<, >, >>) and updates file descriptors.
void set_parent_environment();                                                              // Sets the parent environment variable to the shell's executable path.

// External environment variable array
extern char **environ;

#endif // CUSTOMSHELL_H
