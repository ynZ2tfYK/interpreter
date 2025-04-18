
# Interpreter

A lightweight, modular mathematical expression interpreter written in C, featuring a dynamic plugin system and support for external scripting. This interpreter can evaluate mathematical expressions, handle variables, and extend functionality through shared library plugins.

# Ide and Deps
```bash
sudo apt install gcc libc6-dev make upx-ucl geany
```

Running and Developing on Debian Trixie

## Summary

- gcc: C compiler
- libc6-dev: Standard C library (includes libm, libdl, dirent)
- make: Build tool for Makefile
- upx-ucl: Compresses binaries
- geany: Simple IDE for coding

# Goals

The goal is to create a full scripting interpreter eventually.

## Features

- **Expression Evaluation**: Parse and evaluate mathematical expressions with support for numbers, variables, parentheses, and custom operators.
- **Variable Support**: Define and use variables within expressions or scripts (up to 100 variables).
- **Modular Plugin System**: Dynamically load custom operators (e.g., `+`, `-`, `*`, `/`, `^`) from shared libraries in the `./plugins/` directory (up to 32 plugins).
- **Scripting**: Execute scripts from `.script` files stored in the `scripts/` directory.
- **Error Handling**: Provides meaningful error messages for invalid expressions, undefined variables, and plugin loading issues.
- **Precedence-Based Parsing**: Supports operator precedence for accurate evaluation (e.g., `*` and `/` have higher precedence than `+` and `-`).

## Getting Started

### Prerequisites

- A C compiler (e.g., `gcc`)
- POSIX-compliant system with `dlopen` support (e.g., Linux, macOS)
- Standard C libraries (`math.h`, `dirent.h`, etc.)
- Optional: Shared libraries for plugins (`.so` files)

### Installation

1. Clone the repository:
   ```bash
   git clone https://github.com/ynZ2tfYK/interpreter.git
   cd interpreter
   ```

2. Compile the interpreter: Use our Makefile
   ```bash
   cd interpreter
   make
   ```

3. (Optional) Add custom plugins to the `plugins/` directory as shared libraries (`.so` files).

### Usage

Run the interpreter in interactive mode:
```bash
./interpreter
```

#### Interactive Mode
- Type mathematical expressions (e.g., `2 + 3 * 4`, `x = 5`, `x * 2`).
- Use variables (e.g., `x = 10`, then `x + 5`).
- Exit by typing `exit`.

#### Script Mode
- Create a `.script` file in the `scripts/` directory (e.g., `myscript.script`).
- Run the script by entering its name (e.g., `myscript.script`).
- Scripts support comments (`#`) and one expression per line.

Example script (`scripts/example.script`):
```
# Example script
x = 5
y = x * 2
y + 3
```

Run the script:
```
> test.script
```

#### Example Expressions
```
> 2 + 3 * 4
14.00
> x = 5
> x * 2
10.00
> (1 + 2) * 3
9.00
```

## Plugin System

The interpreter supports a plugin system for extending functionality. Plugins are shared libraries (`.so` files) loaded from the `./plugins/` directory at startup. Each plugin defines a custom operator with:

- A symbol (e.g., `+`, `^`)
- A precedence level (0 for low, 1 for high, 2 for highest)
- An evaluation function (e.g., `double add(double a, double b)`)

### Creating a Plugin

1. Create a C file for the plugin (e.g., `add_plugin.c`):
   ```c
   #include "interpreter.h"

   static double add_func(double a, double b) {
       return a + b;
   }

   static Plugin plugin = {
       .symbol = "+",
       .precedence = 0,
       .eval_func = add_func,
       .token_id = 0,
       .handle = NULL
   };

   Plugin *init_plugin(void) {
       return &plugin;
   }
   ```

2. Compile the plugin into a shared library:
   ```bash
   gcc -shared -fPIC -o plugins/add.so plugins_c/add_plugin.c
   ```
   
3. (Optional) Compile the interpreter and plugins into a shared library using the Makefile.
   ```bash
   make
   ```

3. Restart the interpreter to load the plugin.

The interpreter will automatically detect and load all `.so` files in the `plugins/` directory.

## Project Structure

```
interpreter/
├── interpreter.c  # Main program logic
├── interpreter.h  # Core definitions (tokens, parser, plugins, variables)
├── parse.c       # Parsing logic
├── parse.h       # Parsing interface
├── eval.c        # Expression evaluation logic
├── eval.h        # Evaluation interface
├── plugin_c/      # Directory for plugins
├── plugins/      # Directory for plugin shared libraries (.so)
├── scripts/      # Directory for script files (.script)
```

## Limitations

- Maximum of 256 tokens per expression.
- Maximum of 100 variables.
- Maximum of 32 plugins.
- Variable names limited to 31 characters.
- No support for complex numbers or advanced mathematical functions (unless added via plugins).

## Contributing

Contributions are welcome! To contribute:

1. Fork the repository.
2. Create a feature branch (`git checkout -b feature/your-feature`).
3. Commit your changes (`git commit -m "Add your feature"`).
4. Push to the branch (`git push origin feature/your-feature`).
5. Open a pull request.

Please include tests and documentation for new features.

## License

This project is licensed under the WTFPL License. See the [LICENSE](LICENSE) file for details.

## Acknowledgments

- Built with POSIX dynamic linking (`dlopen`, `dlsym`) for plugin support.
- Inspired by simple expression parsers and modular interpreter designs.
