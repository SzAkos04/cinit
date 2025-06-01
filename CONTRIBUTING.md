# Contributing to cinit

Thank you for considering contributing to this project! The following guide will help you get started.


## Getting Started

1. Fork this repository and clone your fork:
```bash
git clone https://github.com/szakos04/cinit.git
cd cinit
```

2. Create a new branch for your feature or bugfix:
```bash
git checkout -b your-feature-name
```

3. Format your code using clang-format with the .clang-format configuration file in the root directory:
```bash
clang-format -i path/to/your/file.c
```
You can format all your C source and header files before committing:
```bash
find . -name "*.[ch]" -exec clang-format -i {} +
```

4. Make your changes, commit them, and push the branch:
```bash
git add .
git commit -m "Describe your changes"
git push origin your-feature-name
```

5. Open a Pull Request from your branch to main.


## Code Style

Please use the `.clang-format` file provided in the root directory to ensure consistent code style.

Commit messages should be clear and descriptive.

Follow existing code conventions.


## Build & Test

Make sure the project builds in `debug` and in `release` as well:

```bash
make -B
```

```bash
make release
```


## Issues

Check existing issues before opening a new one.

Provide clear details when reporting bugs or suggesting features.


## Code of Conduct

Please be respectful and constructive in all interactions.
