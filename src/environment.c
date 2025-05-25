// environment.c
// Author: Eris Alihakaj

#include "customshell.h"



// Sets the parent environment variable to the shells executable path.
void set_parent_environment() {
    char path[MAX_BUFFER];

    // Get the absolute path of shell executable
    ssize_t len = readlink("/proc/self/exe", path, sizeof(path) - 1); // Read symlink to get executable path
    if (len == -1) {
        perror("Error: readlink failed");
        return;
    }
    path[len] = '\0';  // Null terminate the path string

    // Set the environment variable "parent" to the shells path
    if (setenv("parent", path, 1) != 0) {
        perror("Error: setenv failed");
    }
}


/*
 * Function to expand environment variables in a string
 * This function replaces environment variables (e.g., $HOME) in the input string with their actual values.
 */
void expand_environment_variables(char *input) {
    char buffer[MAX_BUFFER];
    char *start = input;

    // Find the next occurrence of '$' in the input string
    while ((start = strstr(start, "$")) != NULL) {
        // Find the end of the variable name (alphanumeric or underscore)
        char *end = start + 1;
        while (isalnum(*end) || *end == '_') {
            end++;
        }

        // Extract the variable name (e.g., "HOME" from "$HOME")
        char variable_name[MAX_BUFFER];
        strncpy(variable_name, start + 1, end - (start + 1));
        variable_name[end - (start + 1)] = '\0';

        // Get the value of the variable from the environment
        char *value = getenv(variable_name);
        if (value == NULL) {
            value = ""; // If the variable is not set, replace it with an empty string
        }

        // Check for buffer overflow before replacing the variable
        if (strlen(input) - (end - start) + strlen(value) >= MAX_BUFFER) {
            fprintf(stderr, "Error: Buffer overflow while expanding environment variables.\n");
            return;
        }

        // Replace $variable with its value in the input string
        snprintf(buffer, sizeof(buffer), "%.*s%s%s", (int)(start - input), input, value, end);
        strcpy(input, buffer);

        // Move the start pointer forward to avoid infinite loops
        start += strlen(value);
    }
}
