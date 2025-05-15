<p align="center">
  <img src="assets/icon.png" alt="cinit logo" width="20%" />
</p>

# cinit

**cinit** is a lightweight CLI utility for quickly initializing new C or C++ projects.


## Features

- Initialize projects in the current or a new directory
- Supports both C and C++ (C is default)
- Simple, intuitive commands


## Installation

### Clone the repository:

```bash
git clone https://github.com/SzAkos04/cinit
cd cinit
```

### Install the application

```bash
sudo make install
```

#### On Linux

Copies the binary to `/usr/local/bin/` and the man page to `/usr/local/man/man1/`

#### On Windows

You have to add the `build/` directory to your system's environment `PATH` manually.


## Usage
```bash
cinit [create|c] [name] <args>  # Create a project in a new directory
cinit [init|i] ([name]) <args>  # Initialize a project in the current directory
cinit --help                    # Show help message
```

### Commands

| Command       | Description                                              |
| ------------- | -------------------------------------------------------- |
| `create`, `c` | Create a project in a **new** directory                  |
| `init`, `i`   | Initialize a project in the **current** directory        |
| `name`        | Project name (alphanumeric or underscores, max 32 chars) |


### Options

| Option      | Description                   |
| ----------- | ----------------------------- |
| `--c`       | Set language to C *(default)* |
| `--cpp`     | Set language to C++           |
| `--help`    | Show help message             |
| `--version` | Show installed version        |


### Examples

Initialize a C project in the current directory:
```bash
cinit init my_project
```

Create a new C project in a new directory:
```bash
cinit create my_project
```

Create a new C++ project in a new directory:
```bash
cinit create my_project --cpp
```
