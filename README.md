# ONNXCC

**A C++20-based ONNX compiler project.**

ONNXCC is a command-line application built around the ONNX ecosystem, with the goal of providing tools to inspect, execute, compile, and benchmark ONNX models.

The project currently establishes the core application structure, command-line interface, dependency management, and unit-testing infrastructure. Its architecture is designed to support additional ONNX-related functionality as development progresses.

## Features

### Currently Implemented

- **Command-line interface:** Structured subcommand-based CLI.
- **Model dump interface:** Accepts a model path and supports graph-display and verbose flags.
- **Argument validation:** Handles missing arguments, invalid options, and unknown subcommands.
- **Version management:** Provides project version and codename information.
- **Unit testing:** Uses GoogleTest to verify CLI behavior and basic project functionality.
- **CMake integration:** Manages project configuration, compilation, and external dependencies.

### Planned Functionality

The CLI also defines the following commands for future implementation:

- `run` – Execute ONNX models.
- `compile` – Compile ONNX models into executable code.
- `benchmark` – Measure ONNX model performance.

> **Note:** The `dump` command currently validates arguments and reports configuration information. Actual ONNX model loading and graph inspection are not yet implemented. The planned commands are also not currently functional.

## Tech Stack

| Component | Technology |
|---|---|
| Programming Language | C++20 |
| Build System | CMake 3.26+ |
| Model Format | ONNX |
| Serialization | Protocol Buffers |
| CLI Argument Parsing | cxxopts |
| Unit Testing | GoogleTest |

### External Dependencies

Dependencies are managed through CMake's `FetchContent`, so they are downloaded and configured automatically during the initial CMake configuration.

| Dependency | Version |
|---|---|
| ONNX | 1.23.0 |
| Protocol Buffers | 31.1 |
| GoogleTest | 1.15.2 |

### Key Components

- **`main.cpp`** – Application entry point. Delegates command-line processing to the CLI module.
- **`cli/`** – Contains command parsing, option validation, subcommand dispatching, and output handling.
- **`version.cpp`** – Exposes the application version and codename.
- **`version.h.in`** – CMake template used to generate the version header during configuration.
- **`tests/unit/`** – Contains sanity checks and CLI contract tests.
- **`CMakeLists.txt`** – Defines the project, compiler requirements, dependencies, and build targets.

## Prerequisites

Ensure the following tools are installed:

- A C++20-compatible compiler (GCC, Clang, or MSVC).
- CMake 3.26 or later.
- Git.
- An internet connection for downloading dependencies during the initial configuration.

## Getting Started

### 1. Clone the Repository

```bash
git clone https://github.com/Greninja-07/onnxcc-task-Greninja-07.git

cd onnxcc-task-Greninja-07
```

### 2. Configure the Project

```bash
cmake -S . -B build
```

This configures the project and automatically fetches the required external dependencies.

### 3. Build

```bash
cmake --build build --config Release
```

The compiled executable is placed in the build directory.

### 4. Run the Application

**Display available commands:**

```bash
./build/onnxcc --help
```

On Windows, use:

```powershell
.\build\Release\onnxcc.exe --help
```

For single-configuration generators such as Ninja or MinGW Makefiles, the executable is generally located directly inside `build/`, rather than `build/Release/`.

## Command-Line Usage

The general syntax is:

```bash
onnxcc <subcommand> [options]
```

### Available Commands

| Command | Description | Status |
|---|---|---|
| `dump` | Inspect and dump ONNX model details | CLI implemented |
| `run` | Execute an ONNX model | Coming soon |
| `compile` | Compile an ONNX model | Coming soon |
| `benchmark` | Benchmark model performance | Coming soon |

### Dump Command

The `dump` command currently supports the following options:

| Option | Short Form | Description |
|---|---|---|
| `--model` | `-m` | Path to the ONNX model file (required) |
| `--show-graph` | — | Enable the graph-display flag |
| `--verbose` | `-v` | Enable verbose diagnostic output |
| `--help` | `-h` | Display dump command usage |

**Examples:**

Display dump command help:

```bash
onnxcc dump --help
```

Provide a model path:

```bash
onnxcc dump --model model.onnx
```

Enable graph display:

```bash
onnxcc dump --model model.onnx --show-graph
```

Enable verbose output:

```bash
onnxcc dump --model model.onnx --verbose
```

Combine options:

```bash
onnxcc dump --model model.onnx --show-graph --verbose
```

**Verbose output example:**

```text
Model file: model.onnx
Show graph: true
Verbose: true
```

Currently, the model path is accepted as an argument but the command does not open or parse the specified file.

## Testing

The project uses **GoogleTest** and integrates its test suite with CTest.

After building the project, run:

```bash
ctest --test-dir build --output-on-failure
```

Alternatively, execute the test binary directly:

```bash
./build/unit_tests
```

The test suite covers:

- Basic project and version initialization.
- Valid dump command arguments.
- Boolean CLI flags.
- Top-level and subcommand help output.
- Missing required arguments.
- Unknown subcommands.
- Unrecognized command-line options.
- Correct exit codes and error-stream behavior.

## Build Configuration

The project uses CMake to manage compilation and dependencies.

Some notable configuration details:

- Requires C++20 with compiler extensions disabled.
- Enables compiler warnings (`-Wall`, `-Wextra`, `-Wpedantic` for GCC and Clang).
- Uses `/W4` and `/permissive-` for MSVC.
- Exports `compile_commands.json` for tooling such as clangd.
- Builds the ONNXCC library as a static library.
- Builds the CLI executable separately and links it against the library.
- Integrates GoogleTest with CTest for automated testing.
