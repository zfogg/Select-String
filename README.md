# Select-String

A fast, simple grep-like utility for searching text line-by-line from stdin.

## Features

- Fast C implementation with minimal dependencies
- Simple substring matching
- Clean, readable codebase
- Cross-platform (Linux, macOS, Windows)
- Zero configuration required

## Building

### Prerequisites

- A C compiler (clang or gcc)
- Make

### Build Commands

```bash
# Build the binary
make

# Clean build artifacts
make clean

# Show help
make help
```

The compiled binary will be placed in `bin/Select-String`.

## Installation

```bash
# Install to your system PATH
make install
```

The installer will automatically detect the best location:
- `~/bin` (if it exists and is in your PATH)
- `/usr/local/bin` (on Unix-like systems)
- `%SYSTEMROOT%\system32` (on Windows, not recommended)

### Manual Installation

If automatic installation doesn't work, manually copy the binary:

```bash
cp bin/Select-String ~/bin/
# or
sudo cp bin/Select-String /usr/local/bin/
```

Make sure the destination directory is in your `$PATH`.

## Usage

Basic syntax:

```bash
Select-String <pattern>
```

The program reads from stdin and prints lines containing the pattern.

### Examples

```bash
# Search for "error" in a log file
cat app.log | Select-String "error"

# Search for "TODO" in source files
find . -name "*.c" | xargs cat | Select-String "TODO"

# Case-sensitive substring matching
echo "Hello World" | Select-String "Hello"  # Matches
echo "Hello World" | Select-String "hello"  # No match

# Piping with other commands
git log | Select-String "fix:"

# Multiple files with grep-like behavior
cat file1.txt file2.txt | Select-String "pattern"
```

### Options

```bash
# Show help
Select-String --help
Select-String -h

# Show version
Select-String --version
Select-String -v
```

## How It Works

Select-String uses simple substring matching with `strstr()` from the C standard library. This makes it:

- **Fast**: No regex compilation overhead
- **Predictable**: Simple string matching behavior
- **Portable**: Works anywhere with a C99 compiler

## Design Philosophy

This tool is intentionally minimal:

- **No regex**: Simple substring matching only
- **No configuration**: Works out of the box
- **No dependencies**: Just the C standard library
- **Small footprint**: ~200KB binary

If you need advanced features like regex, use `grep`, `ripgrep`, or PowerShell's `Select-String`.

## Differences from PowerShell's Select-String

This is a simplified, faster implementation:

| Feature | This Tool | PowerShell Select-String |
|---------|-----------|--------------------------|
| Speed | Very fast | Slower (PowerShell overhead) |
| Regex | No | Yes |
| Objects | No | Yes (returns MatchInfo objects) |
| Highlighting | No | Yes |
| File input | Via pipes | Direct file arguments |

## Building from Source

The build system uses:
- **Compiler**: Automatically detects `clang` or falls back to `gcc`
- **Flags**: `-std=c99 -Wall -Wextra -Wpedantic -O2 -DNDEBUG`
- **Output**: `bin/Select-String`

## Uninstalling

```bash
make uninstall
```

Or manually remove the binary from your PATH.

## License

This is free and unencumbered software released into the public domain.

## Contributing

This is a personal utility tool, but feel free to fork and modify as needed.

## Why This Exists

Created to provide a fast, simple grep-like command that works consistently across platforms without PowerShell startup overhead.
