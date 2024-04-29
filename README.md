# cinit
A command-line tool created to help you initialize C and C++ projects. Works on Windows, Linux, and MacOS.
It creates a directory with the name of the project and inside it, it creates a src/, include/ and build/ subdirectories.
It also creates a Makefile with the basic structure to compile the project.

## Installation
To install cinit, you can use the following command:
```bash
git clone https://github.com/SzAkos04/cinit.git
cd cinit
make release
```
On Windows, `make release` may give some errors, but the tool should still work.

Optionally, add the `cinit/build` directory to your PATH environment variable to access the tool globally.

## Usage
The tool has three commands:
- `cinit c|create <project_name> --args`: Creates a new project with the given name.
- `cinit i|init (<project_name>) --args`: Initializes the project in the current directory.
- `cinit --help`: Shows the help message.

The `cinit create` command creates a new project with the given name.

The `cinit init` command initializes the project in the current directory.

The `--args` flag is optional, and it can be used to pass additional arguments to the project, such as `--cpp` to create a C++ project instead of the default C one.

The `cinit --help` command shows the help message.

## Example
Create a new project by running the following command:
```bash
cinit c my_project
```
(You can also use the `--cpp` flag to create a C++ project.)

Change to the new project directory:
```bash
cd my_project
```
Compile and run the project:
```bash
make
```
This will create a new project called my_project, and will run it. By default, it will print "Hello, World!" to the console.
