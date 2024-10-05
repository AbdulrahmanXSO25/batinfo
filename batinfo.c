#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/stat.h>

#define VERSION "0.1"

/* Bitmasks for command-line options */
#define OPT_CAPACITY      (1 << 0)
#define OPT_STATUS        (1 << 1)
#define OPT_POWER_SOURCE  (1 << 2)
#define OPT_TIME          (1 << 3)
#define OPT_RATE          (1 << 4)
#define OPT_ALL           (1 << 5)
#define OPT_VERSION       (1 << 6)
#define OPT_HELP          (1 << 7)

/* Color macros for terminal output */
#define COLOR_RESET   "\033[0m"
#define COLOR_GREEN   "\033[92m"
#define COLOR_YELLOW  "\033[93m"
#define COLOR_RED     "\033[91m"

/* File paths for battery information */
#define CAPACITY_PATH "/sys/class/power_supply/BAT0/capacity"
#define STATUS_PATH "/sys/class/power_supply/BAT0/status"
#define POWER_SOURCE_PATH "/sys/class/power_supply/AC/online"
#define ENERGY_FULL_PATH "/sys/class/power_supply/BAT0/energy_full"
#define POWER_NOW_PATH "/sys/class/power_supply/BAT0/power_now"

/* Struct for mapping options to functions */
typedef struct {
    const char *name;
    const char *desc;
    uint16_t bitmask;
    char short_opt;
    const char* long_opt;
    void (*print_func)(void);  /* Function pointer for action */
} Option;

/* Function declarations */
void print_version();
void print_usage(const char* prog_name, Option* options, int options_count);
int file_exists(const char *path);
char* read_file(const char* path);
void print_capacity();
void print_status();
void print_power_source();
void print_time_remaining();
void print_rate();
void print_all();

/* Check if a file exists */
int file_exists(const char *path) {
    struct stat buffer;
    return (stat(path, &buffer) == 0);
}

/* Read a file and return its content as a string */
char* read_file(const char* path) {
    FILE* file = fopen(path, "r");
    if (!file) {
        return NULL;
    }

    char* buffer = malloc(16);
    if (!buffer) {
        fclose(file);
        return NULL;
    }

    if (!fgets(buffer, 16, file)) {
        free(buffer);
        fclose(file);
        return NULL;
    }

    /* Remove newline character */
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n') {
        buffer[len - 1] = '\0';
    }

    fclose(file);
    return buffer;
}

/* Read an integer value from a file */
int read_int_from_file(const char* path) {
    FILE* file = fopen(path, "r");
    if (!file) {
        return -1;
    }

    int value = 0;
    if (fscanf(file, "%d", &value) != 1) {
        fclose(file);
        return -1;
    }

    fclose(file);
    return value;
}

/* Print battery capacity with color coding */
void print_capacity() {
    char* capacity_str = read_file(CAPACITY_PATH);
    if (capacity_str) {
        int capacity = atoi(capacity_str);
        if (capacity >= 50) {
            printf(COLOR_GREEN "Capacity: %d%%" COLOR_RESET "\n", capacity);
        } else if (capacity >= 20) {
            printf(COLOR_YELLOW "Capacity: %d%%" COLOR_RESET "\n", capacity);
        } else {
            printf(COLOR_RED "Capacity: %d%%" COLOR_RESET "\n", capacity);
        }
        free(capacity_str);
    } else {
        printf("Capacity: Unavailable\n");
    }
}

/* Print battery status */
void print_status() {
    char* status = read_file(STATUS_PATH);
    if (status) {
        printf("Status: %s\n", status);
        free(status);
    } else {
        printf("Status: Unavailable\n");
    }
}

/* Print power source (AC or Battery) */
void print_power_source() {
    char* power_source = read_file(POWER_SOURCE_PATH);
    if (power_source && strcmp(power_source, "1") == 0) {
        printf("Power Source: AC\n");
        free(power_source);
    } else {
        printf("Power Source: Battery\n");
        free(power_source);
    }
}

/* Estimate and print remaining time based on discharge rate */
void print_time_remaining() {
    int capacity = read_int_from_file(CAPACITY_PATH);
    if (capacity < 0) {
        printf("Error reading battery capacity.\n");
        return;
    }

    int energy_full = read_int_from_file(ENERGY_FULL_PATH);
    if (energy_full < 0) {
        printf("Error reading full charge capacity.\n");
        return;
    }

    int power_now = read_int_from_file(POWER_NOW_PATH);
    if (power_now < 0) {
        printf("Error reading power consumption.\n");
        return;
    }

    char* status = read_file(STATUS_PATH);
    if (!status) {
        printf("Error reading battery status.\n");
        return;
    }

    if (strcmp(status, "Discharging") == 0) {
        double battery_remaining_wh = (capacity / 100.0) * (energy_full / 1e6);
        double power_w = power_now / 1e6;

        if (power_w > 0) {
            double time_remaining_hours = battery_remaining_wh / power_w;
            int hours = (int)time_remaining_hours;
            int minutes = (int)((time_remaining_hours - hours) * 60);
            printf("Estimated Time Remaining: %d hours %d minutes\n", hours, minutes);
        } else {
            printf("Unable to estimate time remaining.\n");
        }
    } else if (strcmp(status, "Charging") == 0) {
        printf("Battery is charging; time remaining not available.\n");
    } else {
        printf("Battery status is not discharging or charging. Time estimation is unavailable.\n");
    }

    free(status);
}

/* Print charge/discharge rate */
void print_rate() {
    int power_now = read_int_from_file(POWER_NOW_PATH);
    if (power_now < 0) {
        printf("Charge/Discharge Rate: Unavailable\n");
        return;
    }

    double power_w = power_now / 1e6;
    printf("Charge/Discharge Rate: %.2f W\n", power_w);
}

/* Print all available battery information */
void print_all() {
    print_capacity();
    print_status();
    print_power_source();
    print_time_remaining();
    print_rate();
}

/* List of available options */
Option options[] = {
    {"capacity", "Show battery capacity", OPT_CAPACITY, 'p', "capacity", print_capacity},
    {"status", "Show battery status", OPT_STATUS, 's', "status", print_status},
    {"ac", "Show if power source is AC or Battery", OPT_POWER_SOURCE, 'a', "ac", print_power_source},
    {"time", "Show estimated time remaining", OPT_TIME, 't', "time", print_time_remaining},
    {"rate", "Show charge/discharge rate", OPT_RATE, 'r', "rate", print_rate},
    {"all", "Show all information", OPT_ALL, 'x', "all", print_all},
    {"version", "Show version information", OPT_VERSION, 'v', "version", print_version},
    {"help", "Show this help message", OPT_HELP, 'h', "help", NULL}
};

/* Parse command-line arguments into bitmask */
uint16_t parse_args(int argc, char* argv[]) {
    uint16_t options_mask = 0;
    for (int i = 1; i < argc; i++) {
        for (size_t j = 0; j < sizeof(options)/sizeof(Option); j++) {
            if (strcmp(argv[i], options[j].long_opt) == 0 ||
                (argv[i][0] == '-' && argv[i][1] == options[j].short_opt)) {
                options_mask |= options[j].bitmask;
                break;
            }
        }
    }
    return options_mask;
}

/* Print usage message */
void print_usage(const char* prog_name, Option* options, int options_count) {
    printf("Usage: %s [options]\n", prog_name);
    for (int i = 0; i < options_count; i++) {
        printf("  -%c, --%-10s  %s\n", options[i].short_opt, options[i].long_opt, options[i].desc);
    }
}

/* Print version information */
void print_version() {
    printf("batinfo version %s\n", VERSION);
}

int main(int argc, char* argv[]) {
    if (argc == 1) {
        print_usage(argv[0], options, sizeof(options) / sizeof(Option));
        return EXIT_FAILURE;
    }

    uint16_t options_mask = parse_args(argc, argv);

    if (options_mask & OPT_VERSION) {
        print_version();
        return EXIT_SUCCESS;
    }

    if (options_mask & OPT_HELP) {
        print_usage(argv[0], options, sizeof(options) / sizeof(Option));
        return EXIT_SUCCESS;
    }

    for (size_t i = 0; i < sizeof(options) / sizeof(Option); i++) {
        if (options_mask & options[i].bitmask && options[i].print_func != NULL) {
            options[i].print_func();
        }
    }

    return EXIT_SUCCESS;
}
