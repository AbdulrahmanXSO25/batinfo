# Batinfo

**Version:** 0.1

Batinfo is a simple command-line tool designed to retrieve battery information on Linux systems. It allows users to view battery capacity, status, power source, and more.

## Features

- Display battery capacity percentage.
- Show battery status (charging, discharging, full).
- Display power source (AC or Battery).
- Estimate time remaining based on current discharge rate.
- Show current charge/discharge rate in watts.
- Display all available battery information in one command.

## Installation

1. Clone the repository and navigate into the directory:
   ```bash
   git clone https://github.com/your-username/batinfo.git
   cd batinfo
   ```
2. Install the program and the man page using `make`:
   ```bash
   sudo make clean install
   ```
3. Verify by running:
   ```bash
   batinfo -h
   ```

## Usage

```bash
Usage: batinfo [options]
  -p, --capacity   Show battery capacity
  -s, --status     Show battery status
  -a, --ac         Show if power source is AC or Battery
  -t, --time       Show estimated time remaining
  -r, --rate       Show charge/discharge rate
  -x, --all        Show all information
  -v, --version    Show version information
  -h, --help       Show this help message
```

## License

This project is licensed under the BSD 2-Clause License - see the [LICENSE](LICENSE) file for details.
