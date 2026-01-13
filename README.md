# GPA Calculator

A simple, command-line GPA calculator written in C. This tool allows you to manage your course grades and calculate your GPA with a clean and colorful terminal interface.

![Screenshot](./screenshot.png)

## Features

- **Add courses** with credits and (grades)
- **Remove courses** from your database
- **List all courses** in a formatted table
- **Calculate GPA** using standard grade point values from entries with grade
- **Calculate total credits** including entries with no grade
- **Data saving** - data is automatically saved locally to `./data.txt`

## Usage

### Running Locally

Compile the program in the project directory:

```bash
g++ -Wall -pedantic -Wextra -fsanitize=address -g main.c -o main
```
And run it with:

```bash
./main
```

## Commands

| Command | Description | Example |
|---------|-------------|---------|
| `+` | Add a new course entry | `+ MATH101 3 A` |
| `-` | Remove a course entry | `- MATH101` |
| `?` | List all courses in a formatted table | `?` |
| `=` | Calculate and display current GPA | `=` |
| `#` | Show the number of entries | `#` |

### Command Format

- **Add entry**: `+ [course_name] [grade] [credits]`
  - `course_name`: Course identifier (up to 100 characters, no spaces)
  - `credits`: Number of credits (1-99)
  - `grade`: Single letter grade (A, B, C, D, E, or F) (optional, if not specified it won't be used to calculate GPA)

- **Remove entry**: `- [course_name]`
  - `course_name`: Course identifier to remove


## Features i could probably add

- Saving data to a database
- Encrypting data to make it more secure
- Some more functions fx to manage individual courses and points
