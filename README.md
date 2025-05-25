# CustomShell - Stage 2
- **Author**: Eris Alihakaj
- **Student_ID**: 23305593
- **Acknowledgement**: I acknowledge DCU's Academic Integrity Policy.

## Overview
This is Stage 2 of my custom shell, which I developed as part of the CSC1021 Operating Systems module. CustomShell is a simple command-line shell implemented in C. It supports a variety of internal commands, batch mode execution, I/O redirection, background execution, and environment variable expansion. This stage builds upon Stage 1, adding advanced features to enhance functionality and usability.

---

## Features

- **Internal Commands**:
  - `cd <directory>`: Change the current directory. If no directory is provided, print the current directory.
  - `clr`: Clear the terminal screen.
  - `dir <directory>`: List the contents of the specified directory (or current directory if none is specified).
  - `environ`: List all environment variables.
  - `echo <text>`: Print the given text to the console.
  - `help`: Display the user manual.
  - `pause`: Pause the shell until Enter is pressed.
  - `quit`: Exit the shell.

- **Batch Mode**:
  - The shell can process commands from a batch file. For example:
    ```bash
    ./customshell batchfile.txt
    ```
  - Here is an example of a batch file:
    ```bash
    clr
    cd /tmp
    dir
    echo Hello, World!
    pause
    help
    quit

### **Advanced Features**:
- **I/O Redirection**:
  - `<`: Redirect input from a file.  
    Example: `command < input.txt`
  - `>`: Redirect output to a file (overwrites the file).  
    Example: `command > output.txt`
  - `>>`: Redirect output to a file (appends to the file).  
    Example: `command >> output.txt`

- **Background Execution**:
  - Add `&` at the end of a command to run it in the background.  
    Example: `sleep 10 &`

- **Environment Variables**:
  - `shell` and `parent` store the full path of the CustomShell executable.
    Example: `echo $parent` prints `/home/user/customshell/bin/customshell`.


- **Error Handling**:
  - The shell handles errors such as invalid directories, missing files, and invalid commands.


## Examples
- **I/O Redirection**:
  - echo Hello > output.txt: Writes "Hello" to output.txt.

  - dir >> log.txt: Appends the directory listing to log.txt.

  - sort < input.txt: Sorts the contents of input.txt.

- **Background Execution**:
  - sleep 10 &: Runs sleep 10 in the background.


- **Environment Variable Expansion**:
  - echo Current user: $USER: Prints the current user's username.



## User Manual
For a detailed user manual, run the help command in the shell or refer to the manual.txt file included in the project.



## Getting Started

### Prerequisites
- A Linux environment (e.g., WSL, native Linux, or a Linux VM).
- GCC compiler.

### Building the Shell
1. Clone the repository:
   ```bash
   git clone https://gitlab.computing.dcu.ie/your-repo.git
2. Navigate to the project directory:
   ```bash
   cd 2025-customshell-23305593-erisalihakaj/Stage1
3. Build the shell by running:
   ``` bash
   make 
4. Run the shell executable:
   ``` bash 
   ./bin/customshell
5. To clean up and remove the compiled binary files, use:
   ```bash
   make remove