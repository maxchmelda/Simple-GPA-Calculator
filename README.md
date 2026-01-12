# GPA Calculator

A simple, command-line GPA (Grade Point Average) calculator written in C. This tool allows you to manage your course grades and calculate your GPA interactively with a clean terminal interface.

![Screenshot description](./screenshot.png)

## Features

- ✅ **Add courses** with grades and credit hours
- ✅ **Remove courses** from your database
- ✅ **List all courses** in a formatted table
- ✅ **Calculate GPA** using standard grade point values
- ✅ **Persistent storage** - data is automatically saved to `./data.txt`
- ✅ **Color-coded output** for better readability
- ✅ **Input validation** with helpful error messages
- ✅ **Automatic sorting** by course name

## Grade Scale

The calculator uses the following grade point values:

| Grade | Points |
|-------|--------|
| A     | 4.0    |
| B     | 3.0    |
| C     | 2.0    |
| D     | 1.0    |
| E     | 0.5    |
| F     | 0.0    |

## Installation

### Prerequisites

- A C compiler (G++, Clang, etc.)
- A Unix-like terminal (Linux, macOS, or WSL on Windows)

### Compilation

```bash
g++ -Wall -pedantic -Wextra -fsanitize=address -g main.c -o main
```

Or with additional compiler flags for better optimization:

```bash
g++ -Wall -pedantic -Wextra -fsanitize=address -g main.c -o main
```

### System-wide Installation (Optional)

To install the GPA calculator system-wide so you can run it from anywhere using the `gpa` command:

1. Compile the program (see above)

2. Move the executable `./main` to `/usr/local/bin/gpa`:
   ```bash
   sudo mv ./main /usr/local/bin/gpa
   ```

3. Make sure `/usr/local/bin` is in your PATH (it usually is by default):
   ```bash
   echo $PATH | grep -q /usr/local/bin && echo "PATH is set correctly" || echo "Add /usr/local/bin to your PATH"
   ```

4. Verify the installation:
   ```bash
   which gpa
   gpa --version  # or just run 'gpa' to start the program
   ```

Now you can run the GPA calculator from any directory simply by typing:
```bash
gpa
```

**Note**: You may need `sudo` privileges to write to `/usr/local/bin`. If you prefer not to use `sudo`, you can install it to `~/bin` instead (create the directory first with `mkdir -p ~/bin`), but make sure `~/bin` is in your PATH.

## Usage

### Running Locally

If you compiled the program in the project directory, run it with:

```bash
./main
```

## Commands

| Command | Description | Example |
|---------|-------------|---------|
| `+` | Add a new course entry | `+ MATH101 A 3` |
| `-` | Remove a course entry | `- MATH101` |
| `?` | List all courses in a formatted table | `?` |
| `=` | Calculate and display current GPA | `=` |
| `#` | Show the number of entries | `#` |

### Command Format

- **Add entry**: `+ [course_name] [grade] [credits]`
  - `course_name`: Course identifier (up to 100 characters, no spaces)
  - `grade`: Single letter grade (A, B, C, D, E, or F)
  - `credits`: Number of credits (1-99)

- **Remove entry**: `- [course_name]`
  - `course_name`: Course identifier to remove

## Examples

### Adding Courses

```
>> + CS101 A 4
[+] Entry [CS101] was successfully added.

>> + MATH201 B 3
[+] Entry [MATH201] was successfully added.

>> + PHYS101 C 4
[+] Entry [PHYS101] was successfully added.
```

### Viewing All Courses

```
>> ?
+----------------------+--------+---------+
| Course               | Grade  | Credits |
+----------------------+--------+---------+
| CS101                |   A    |    4    |
| MATH201              |   B    |    3    |
| PHYS101              |   C    |    4    |
+----------------------+--------+---------+
```

### Calculating GPA

```
>> =
╔════════════════════════════════════╗
║        Current student's GPA       ║
╠════════════════════════════════════╣
║             >  3.09  <             ║
╚════════════════════════════════════╝
```

### Removing a Course

```
>> - PHYS101
[-] Entry [PHYS101] was removed from the database.
```

## Data Storage

All course data is automatically saved to `data.txt` in the following format:

```
COURSE_NAME GRADE CREDITS
```

Example:
```
CS101 A 4
MATH201 B 3
PHYS101 C 4
```

The file is created automatically when you add your first course and updated whenever you add or remove entries.

## Error Handling

The program includes comprehensive error handling:

- Invalid grade letters
- Invalid credit values (must be 1-99)
- Duplicate course entries
- Missing course names
- Incomplete command formats

All errors are displayed with clear, color-coded messages.

## Technical Details

- **Language**: C
- **Data Structure**: Dynamic array with automatic resizing
- **Sorting**: Uses `qsort` and `bsearch` for efficient course lookup
- **File I/O**: Simple text-based persistence
- **Memory Management**: Manual memory allocation with proper cleanup

## License

This project is open source and available for personal and educational use.

## Contributing

Contributions, issues, and feature requests are welcome! Feel free to fork this project and submit pull requests.
