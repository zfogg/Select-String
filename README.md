# Select-String

A lightweight C wrapper for PowerShell's Select-String cmdlet, providing easy access to PowerShell's powerful text search capabilities from any shell.

## Features

- **Full PowerShell Select-String support**: Access all PowerShell Select-String features
- **Regex matching**: Supports regular expressions via PowerShell
- **Case sensitivity**: `-CaseSensitive` flag support
- **File input**: Direct file arguments with `-Path` parameter
- **Piped input**: Works seamlessly with stdin
- **Comprehensive error handling**: Validates PowerShell availability and buffer limits
- **Windows native**: Built for Windows with PowerShell

## Requirements

- **PowerShell**: Must be installed and available in PATH
- **Windows**: Designed for Windows (uses Windows-specific APIs)
- **C compiler**: gcc (MinGW/MSYS2) or compatible
- **Make**: For building from source

### Build Commands

```bash
# Build the binary
make

# Clean build artifacts
make clean

# Show help
make help
```

The compiled binary will be placed in `bin/Select-String` (note: the Makefile creates it without the .exe extension, but it's still a Windows executable).

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

This wrapper forwards all arguments to PowerShell's `Select-String` cmdlet, so you can use any Select-String parameters.

Basic syntax:

```bash
Select-String <pattern> [PowerShell Select-String arguments]
```

### Examples

```bash
# Search for "error" in piped input
echo "error occurred" | Select-String "error"

# Search files directly with -Path
Select-String "error" -Path *.log

# Case-sensitive search
Select-String "ERROR" -Path app.log -CaseSensitive

# Regular expressions (default)
Select-String "error|warning" -Path *.log

# Search with Get-Content piping
Get-Content app.log | Select-String "critical"

# Complex patterns
Select-String "\d{3}-\d{4}" -Path contacts.txt  # Phone numbers

# Search multiple file types
Select-String "TODO" -Path *.c,*.h

# Context lines (lines before/after match)
Select-String "error" -Path app.log -Context 2,3
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

This is a C wrapper that:

1. **Detects piped input**: Checks if stdin is a pipe using `_isatty()`
2. **Handles piped data**: Saves stdin to a temporary file and uses PowerShell's `Get-Content` to pipe it to `Select-String`
3. **Forwards arguments**: Passes all command-line arguments to PowerShell's `Select-String` cmdlet
4. **Calls PowerShell**: Executes `powershell.exe -NoProfile -Command "...Select-String <args>"`
5. **Returns output**: Streams PowerShell's output back to stdout

### Error Handling

The wrapper includes comprehensive error checking:

- **PowerShell availability**: Validates PowerShell exists in PATH before execution
- **Buffer overflow protection**: Checks command string doesn't exceed 32,768 bytes
- **snprintf validation**: Verifies all string formatting operations succeed
- **File I/O errors**: Validates reads/writes to temporary files
- **Pipe errors**: Checks for errors reading PowerShell output

## Design Philosophy

This wrapper provides:

- **Transparent PowerShell access**: Full access to PowerShell Select-String features
- **Shell integration**: Works from bash, PowerShell, or any shell environment
- **Simple interface**: Just pass arguments as you would to Select-String
- **Robust error handling**: Comprehensive validation and helpful error messages
- **Resource cleanup**: Properly manages temporary files and handles

## PowerShell Select-String Features

Since this is a wrapper, you get full access to PowerShell's Select-String:

| Feature | Supported |
|---------|-----------|
| Regex patterns | ✅ Yes |
| Case-sensitive matching | ✅ Yes (`-CaseSensitive`) |
| File path patterns | ✅ Yes (`-Path`) |
| Context lines | ✅ Yes (`-Context`) |
| Line numbers | ✅ Yes (in output) |
| Simple matching | ✅ Yes (`-SimpleMatch`) |
| Multiple patterns | ✅ Yes (via regex) |
| Recursive search | ✅ Yes (use Get-ChildItem with piping) |

## Building from Source

The build system uses:
- **Compiler**: gcc (MinGW/MSYS2 recommended)
- **Flags**: `-std=c99 -Wall -Wextra -Wpedantic -O2 -DNDEBUG`
- **Output**: `bin/Select-String` (Windows executable)
- **Binary size**: ~240KB

### Technical Details

- Uses Windows-specific APIs: `_isatty()`, `_popen()`, `_tempnam()`
- Command buffer: 32,768 bytes
- Data buffer: 8,192 bytes
- Temporary file handling for piped input

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

This wrapper makes PowerShell's `Select-String` easily accessible from any shell environment (bash, cmd, etc.) without needing to write PowerShell syntax. It handles the complexity of:

- Detecting and managing piped input
- Properly escaping and formatting PowerShell commands
- Managing temporary files for stdin data
- Providing helpful error messages
- Cleaning up resources

You get the power of PowerShell's regex-enabled search with the simplicity of a traditional command-line tool.

## Limitations

- **Windows only**: Uses Windows-specific APIs
- **Requires PowerShell**: Must have PowerShell installed and in PATH
- **Command length**: Limited to 32,768 bytes for the command string
- **Startup time**: Includes PowerShell startup overhead (typically ~100-200ms)
