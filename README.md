# Shell C++ Implementation

A POSIX-compliant shell implementation written in modern C++ (C++23) that provides standard shell functionality with support for command execution, built-in commands, and I/O redirection.

## Features

### ✅ Implemented Features

#### POSIX Shell Compliance
- **Interactive Command Prompt**: Standard `$ ` prompt with line-by-line input processing
- **POSIX Command Tokenization**: Standards-compliant tokenizer supporting:
  - Single quotes (`'text'`) - literal interpretation per POSIX
  - Double quotes (`"text"`) - parameter expansion context
  - Escape sequences (`\n`, `\t`, `\\`) following POSIX rules
  - Mixed quoting scenarios as per shell grammar

#### POSIX Built-in Commands
- **`exit [code]`**: Exit shell with optional exit status (POSIX.1-2017)
- **`echo [args...]`**: Write arguments to standard output
- **`type <command>`**: Display command classification (builtin/external)
- **`pwd`**: Print current working directory pathname
- **`cd [directory]`**: Change working directory (supports `~` expansion)

#### External Command Execution
- **PATH Resolution**: Standard PATH environment variable processing
- **Process Management**: POSIX fork/exec model implementation
- **Wait Operations**: Proper child process reaping with `waitpid()`
- **Exit Status**: Standard exit code propagation

#### I/O Redirection (POSIX)
- **Output Redirection**:
  - `>` - Redirect stdout (create/truncate)
  - `1>` - Explicit stdout redirect
  - `>>` - Redirect stdout (append)
  - `1>>` - Explicit stdout append

- **Error Redirection**:
  - `2>` - Redirect stderr (create/truncate)
  - `2>>` - Redirect stderr (append)

#### Advanced Parsing
- **POSIX Quote Removal**: Proper quote processing per shell grammar
- **Escape Processing**: Context-sensitive backslash handling
- **Syntax Validation**: Error detection for malformed commands

## Architecture

### Modular Design
```
src/
├── main.cpp          # Main shell loop and redirection logic
├── tokenize.cpp/hpp  # POSIX-compliant tokenization
├── builtins.cpp/hpp  # Built-in command implementations
└── util.cpp/hpp      # External command execution utilities
```

### POSIX Compliance
- Follows POSIX.1-2017 shell specification
- Standard file descriptor handling (0=stdin, 1=stdout, 2=stderr)
- Proper signal handling for interactive use
- Environment variable inheritance

## Building and Running

### Prerequisites
- CMake 3.13+
- POSIX-compliant system (Linux, macOS, Unix)
- C++23 compatible compiler

### Build Instructions
```bash
cd shell-cpp
./your_program.sh
```

## Usage Examples

### POSIX Shell Operations
```bash
$ echo "Hello, World!"
Hello, World!

$ pwd
/home/user/projects

$ type echo
echo is a shell builtin

$ ls nonexistent 2> /dev/null
$ echo $?
2
```

## Future Features

### 🔲 Command Line Enhancement
- [ ] Tab completion for commands and paths
- [ ] Command history with up/down arrows
- [ ] Reverse search (Ctrl+R)
- [ ] History expansion (`!!`, `!n`)

### 🔲 POSIX Pipeline Support  
- [ ] Command pipelines (`cmd1 | cmd2`)
- [ ] Pipeline exit status handling
- [ ] Multi-stage pipeline support

### 🔲 Interactive Features
- [ ] Line editing capabilities
- [ ] Syntax highlighting
- [ ] Auto-suggestion
- [ ] Configuration file support (`.shellrc`)

## Technical Details

### POSIX Compliance
- Implements core POSIX shell features
- Standard exit codes and error handling
- Proper signal disposition
- Environment variable management

### System Integration
- **File Operations**: POSIX file I/O (`open()`, `dup2()`, `close()`)
- **Process Control**: Standard process creation (`fork()`, `exec()`)
- **Signal Handling**: POSIX signal management
- **Error Reporting**: Standard errno-based error handling

---

*A standards-compliant shell implementation showcasing modern C++ design patterns while adhering to POSIX shell specifications.*