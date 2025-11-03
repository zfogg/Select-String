/*
 * Select-String - PowerShell Select-String wrapper
 *
 * Calls PowerShell's Select-String command with all arguments passed through.
 * Handles both piped input and file arguments.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <fcntl.h>

#define BUFFER_SIZE 8192
#define COMMAND_SIZE 32768
#define PROGRAM_NAME "Select-String"
#define VERSION "1.0.0"

static void print_usage(const char *program_name) {
    fprintf(stderr, "Usage: %s [PowerShell Select-String arguments]\n", program_name);
    fprintf(stderr, "Wrapper for PowerShell's Select-String command.\n");
    fprintf(stderr, "\nExamples:\n");
    fprintf(stderr, "  echo \"hello world\" | %s \"hello\"\n", program_name);
    fprintf(stderr, "  %s \"pattern\" -Path *.txt\n", program_name);
    fprintf(stderr, "  %s \"error\" file.log\n", program_name);
}

static void print_version(void) {
    printf("%s version %s (PowerShell wrapper)\n", PROGRAM_NAME, VERSION);
}

static int check_powershell_available(void) {
    // Try to execute a simple PowerShell command to verify it's available
    FILE *test_pipe = _popen("powershell.exe -NoProfile -Command \"exit 0\" 2>nul", "r");
    if (test_pipe == NULL) {
        return 0;  // Failed to execute
    }

    int exit_code = _pclose(test_pipe);
    return (exit_code == 0);  // Return 1 if successful, 0 otherwise
}

int main(int argc, char *argv[]) {
    // Parse arguments
    if (argc < 2) {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
        print_usage(argv[0]);
        return EXIT_SUCCESS;
    }

    if (strcmp(argv[1], "--version") == 0 || strcmp(argv[1], "-v") == 0) {
        print_version();
        return EXIT_SUCCESS;
    }

    // Check if PowerShell is available in PATH
    if (!check_powershell_available()) {
        fprintf(stderr, "Error: PowerShell not found in PATH\n");
        fprintf(stderr, "\n");
        fprintf(stderr, "This program requires PowerShell to be installed and available in your PATH.\n");
        fprintf(stderr, "Please ensure PowerShell is installed and accessible from the command line.\n");
        fprintf(stderr, "\n");
        fprintf(stderr, "To verify PowerShell installation, try running:\n");
        fprintf(stderr, "  powershell.exe -Command \"$PSVersionTable.PSVersion\"\n");
        return EXIT_FAILURE;
    }

    // Build PowerShell command with all arguments
    char command[COMMAND_SIZE];
    int cmd_len = 0;
    int written = 0;

    // Check if stdin is piped
    int is_piped = !_isatty(_fileno(stdin));

    if (is_piped) {
        // Use $input to receive piped data
        written = snprintf(command, COMMAND_SIZE,
            "powershell.exe -NoProfile -Command \"$input | Microsoft.PowerShell.Utility\\Select-String");
    } else {
        // Direct command without piped input
        written = snprintf(command, COMMAND_SIZE,
            "powershell.exe -NoProfile -Command \"Microsoft.PowerShell.Utility\\Select-String");
    }

    // Check if snprintf failed or would overflow
    if (written < 0) {
        fprintf(stderr, "Error: Failed to format command string (snprintf encoding error)\n");
        return EXIT_FAILURE;
    }
    if (written >= COMMAND_SIZE) {
        fprintf(stderr, "Error: Command string too long (initial command: %d bytes, max: %d bytes)\n",
                written, COMMAND_SIZE - 1);
        return EXIT_FAILURE;
    }
    cmd_len = written;

    // Append all arguments
    for (int i = 1; i < argc; i++) {
        // Escape quotes in arguments
        const char *arg = argv[i];
        int needs_quotes = 0;

        // Check if argument contains spaces
        if (strchr(arg, ' ') != NULL) {
            needs_quotes = 1;
        }

        if (needs_quotes) {
            written = snprintf(command + cmd_len, COMMAND_SIZE - cmd_len, " '%s'", arg);
        } else {
            written = snprintf(command + cmd_len, COMMAND_SIZE - cmd_len, " %s", arg);
        }

        // Check if snprintf failed or would overflow
        if (written < 0) {
            fprintf(stderr, "Error: Failed to format argument (snprintf encoding error)\n");
            return EXIT_FAILURE;
        }
        if (written >= COMMAND_SIZE - cmd_len) {
            fprintf(stderr, "Error: Command string too long (after adding argument %d)\n", i);
            fprintf(stderr, "Current length: %d bytes, attempted to add: %d bytes, max: %d bytes\n",
                    cmd_len, written, COMMAND_SIZE - 1);
            return EXIT_FAILURE;
        }
        cmd_len += written;
    }

    // Close the PowerShell command
    written = snprintf(command + cmd_len, COMMAND_SIZE - cmd_len, "\"");
    if (written < 0) {
        fprintf(stderr, "Error: Failed to close command string (snprintf encoding error)\n");
        return EXIT_FAILURE;
    }
    if (written >= COMMAND_SIZE - cmd_len) {
        fprintf(stderr, "Error: Command string too long (cannot add closing quote)\n");
        return EXIT_FAILURE;
    }
    cmd_len += written;

    FILE *pipe = NULL;
    char *temp_file = NULL;

    // If stdin is piped, save it to a temporary file
    if (is_piped) {
        // Create temporary file
        temp_file = _tempnam(NULL, "ss_");
        if (temp_file == NULL) {
            fprintf(stderr, "Error: Failed to create temporary file\n");
            return EXIT_FAILURE;
        }

        FILE *temp = fopen(temp_file, "wb");
        if (temp == NULL) {
            fprintf(stderr, "Error: Failed to open temporary file\n");
            free(temp_file);
            return EXIT_FAILURE;
        }

        // Copy stdin to temp file
        char buffer[BUFFER_SIZE];
        size_t bytes_read;
        size_t bytes_written;
        while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, stdin)) > 0) {
            bytes_written = fwrite(buffer, 1, bytes_read, temp);
            if (bytes_written != bytes_read) {
                fprintf(stderr, "Error: Failed to write data to temporary file\n");
                fprintf(stderr, "Attempted to write %zu bytes, only wrote %zu bytes\n",
                        bytes_read, bytes_written);
                fclose(temp);
                remove(temp_file);
                free(temp_file);
                return EXIT_FAILURE;
            }
        }

        // Check for read errors
        if (ferror(stdin)) {
            fprintf(stderr, "Error: Failed to read from stdin\n");
            fclose(temp);
            remove(temp_file);
            free(temp_file);
            return EXIT_FAILURE;
        }

        fclose(temp);

        // Rebuild command to use Get-Content with the temp file
        written = snprintf(command, COMMAND_SIZE,
            "powershell.exe -NoProfile -Command \"Get-Content -Raw '%s' | Microsoft.PowerShell.Utility\\Select-String",
            temp_file);

        // Check if snprintf failed or would overflow
        if (written < 0) {
            fprintf(stderr, "Error: Failed to format command with temp file (snprintf encoding error)\n");
            remove(temp_file);
            free(temp_file);
            return EXIT_FAILURE;
        }
        if (written >= COMMAND_SIZE) {
            fprintf(stderr, "Error: Command string too long (rebuild with temp file: %d bytes, max: %d bytes)\n",
                    written, COMMAND_SIZE - 1);
            fprintf(stderr, "Temp file path may be too long: %s\n", temp_file);
            remove(temp_file);
            free(temp_file);
            return EXIT_FAILURE;
        }
        cmd_len = written;

        // Append all arguments again
        for (int i = 1; i < argc; i++) {
            const char *arg = argv[i];
            int needs_quotes = (strchr(arg, ' ') != NULL);

            if (needs_quotes) {
                written = snprintf(command + cmd_len, COMMAND_SIZE - cmd_len, " '%s'", arg);
            } else {
                written = snprintf(command + cmd_len, COMMAND_SIZE - cmd_len, " %s", arg);
            }

            // Check if snprintf failed or would overflow
            if (written < 0) {
                fprintf(stderr, "Error: Failed to format argument in rebuild (snprintf encoding error)\n");
                remove(temp_file);
                free(temp_file);
                return EXIT_FAILURE;
            }
            if (written >= COMMAND_SIZE - cmd_len) {
                fprintf(stderr, "Error: Command string too long in rebuild (after adding argument %d)\n", i);
                fprintf(stderr, "Current length: %d bytes, attempted to add: %d bytes, max: %d bytes\n",
                        cmd_len, written, COMMAND_SIZE - 1);
                remove(temp_file);
                free(temp_file);
                return EXIT_FAILURE;
            }
            cmd_len += written;
        }

        // Close the command string
        written = snprintf(command + cmd_len, COMMAND_SIZE - cmd_len, "\"");
        if (written < 0) {
            fprintf(stderr, "Error: Failed to close command string in rebuild (snprintf encoding error)\n");
            remove(temp_file);
            free(temp_file);
            return EXIT_FAILURE;
        }
        if (written >= COMMAND_SIZE - cmd_len) {
            fprintf(stderr, "Error: Command string too long in rebuild (cannot add closing quote)\n");
            remove(temp_file);
            free(temp_file);
            return EXIT_FAILURE;
        }
        cmd_len += written;
    }

    // Open pipe to PowerShell (read mode)
    pipe = _popen(command, "r");
    if (pipe == NULL) {
        fprintf(stderr, "Error: Failed to execute PowerShell\n");
        if (temp_file) {
            remove(temp_file);
            free(temp_file);
        }
        return EXIT_FAILURE;
    }

    // Read and output results from PowerShell
    char buffer[BUFFER_SIZE];
    while (fgets(buffer, BUFFER_SIZE, pipe) != NULL) {
        if (printf("%s", buffer) < 0) {
            fprintf(stderr, "Error: Failed to write output to stdout\n");
            _pclose(pipe);
            if (temp_file) {
                remove(temp_file);
                free(temp_file);
            }
            return EXIT_FAILURE;
        }
        fflush(stdout);
    }

    // Check if loop ended due to error or EOF
    if (ferror(pipe)) {
        fprintf(stderr, "Error: Failed to read output from PowerShell\n");
        _pclose(pipe);
        if (temp_file) {
            remove(temp_file);
            free(temp_file);
        }
        return EXIT_FAILURE;
    }

    // Close pipe and get exit code
    int exit_code = _pclose(pipe);

    // Clean up temp file if it was created
    if (temp_file) {
        remove(temp_file);
        free(temp_file);
    }

    return exit_code;
}
