// io_redirection.c
// Author: Eris Alihakaj
// Student_ID: 23305593
// I Acknowledge DCU's Academic Integrity Policy 

#include "customshell.h"

// Handles I/O redirection (<, >, >>) and updates file descriptors.
int handle_io_redirection(char *args[], int *input_fd, int *output_fd, int *append_mode) {
    int i = 0;
    while (args[i] != NULL) {
        if (strcmp(args[i], "<") == 0) {
            // Input redirection: open file for reading
            if (args[i + 1] == NULL) {
                fprintf(stderr, "Error: No input file specified.\n");
                return -1;
            }

            // Check if the file path exceeds MAX_BUFFER
            if (strlen(args[i + 1]) >= MAX_BUFFER) {
                fprintf(stderr, "Error: Input file path exceeds maximum allowed length.\n");
                return -1;
            }

            *input_fd = open(args[i + 1], O_RDONLY); // Open input file
            if (*input_fd == -1) {
                perror("open");
                return -1;
            }
            args[i] = NULL; // Remove "<" and filename from args
        } 
        else if (strcmp(args[i], ">") == 0) {
            // Output redirection (truncate): open file for writing, overwrite if exists
            if (args[i + 1] == NULL) {
                fprintf(stderr, "Error: No output file specified.\n");
                return -1;
            }

            // Check if the file path exceeds MAX_BUFFER
            if (strlen(args[i + 1]) >= MAX_BUFFER) {
                fprintf(stderr, "Error: Output file path exceeds maximum allowed length.\n");
                return -1;
            }

            *output_fd = open(args[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644); // Open output file
            if (*output_fd == -1) {
                perror("open");
                return -1;
            }
            args[i] = NULL; // Remove ">" and filename from args
        } 
        else if (strcmp(args[i], ">>") == 0) {
            // Output redirection (append): open file for writing, append if exists
            if (args[i + 1] == NULL) {
                fprintf(stderr, "Error: No output file specified.\n");
                return -1;
            }

            // Check if the file path exceeds MAX_BUFFER
            if (strlen(args[i + 1]) >= MAX_BUFFER) {
                fprintf(stderr, "Error: Output file path exceeds maximum allowed length.\n");
                return -1;
            }

            *output_fd = open(args[i + 1], O_WRONLY | O_CREAT | O_APPEND, 0644); // Open output file in append mode
            if (*output_fd == -1) {
                perror("open");
                return -1;
            }
            args[i] = NULL; // Remove ">>" and filename from args
            *append_mode = 1; // Set append mode
        }
        i++;
    }
    return 0; // Success
}