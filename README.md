# ARM Architecture Simulator (for learning, WIP)

A simple ARM architecture simulator implemented in C++. This project simulates the execution of ARM instructions and provides a framework for understanding ARM architecture.

This project is forked from [alxshine/arm-simulator](https://github.com/alxshine/arm-simulator).
Thank you to the original author for creating this wonderful project that has helped me learn about ARM architecture.

## Features

- ARM instruction decoding and execution
- Memory simulation
- ELF file loading
- Register manipulation

## Building the Project

The project uses clang++ as the default compiler. You can build it using VS Code tasks:

1. Bring up build task menu (default shortcut: Ctrl+Shift+B)
2. Select "build arm-sim" to build the entire project
3. Select an optimization level when prompted

Alternatively, you can run individual tasks:
- "compile cpu" - Compile the CPU component
- "build libsim" - Build the simulator library
- "clean" - Remove all build artifacts

## Running the Simulator

After building, you can run the simulator with:

```
./build/arm-sim <elf-file>
```

Where `<elf-file>` is the path to an ARM ELF executable.

## Project Structure

- `include/` - Header files
- `src/` - Source files
- `build/` - Build artifacts
- `main.cpp` - Entry point
