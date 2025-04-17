# C Shell

## Overview
This project implements a simple Unix‑like command shell in C. It supports:

- **Built‑in commands**: `cd`, `exit`  
- **External commands**: invoking any executable in `PATH` (e.g., `ls`, `grep`)  
- **Pipes**: chaining commands with `|`  
- **I/O redirection**: input (`<`), output overwrite (`>`), and append (`>>`)  
- **Background execution**: using `&`  

The shell parses user input, handles whitespace and quoting, sets up pipelines or redirections, and manages child processes.

## Features
- Command parsing with support for arguments, pipelines, and redirections  
- Execution of both built‑in and external commands  
- Job control for background processes  
- Error handling and user‑friendly prompts  

## Prerequisites
- POSIX‑compatible OS (Linux, macOS)  
- GCC (or any C99‑compatible compiler)  
- GNU Make  

## Build & Clean
```bash
make         # Compiles the shell executable (`shell`)
make clean   # Removes object files and the `shell` executable
```

For more details on the project structure and design decisions, please refer to the [ARCHITECTURE.md](architecture.md) file.