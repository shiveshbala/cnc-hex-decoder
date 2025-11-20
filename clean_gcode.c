#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_LINE 512

const char* get_hex(const char* line) {
    static char hex[17] = {0};
    const char* open = strchr(line, '(');
    const char* close = open ? strchr(open, ')') : NULL;

    if (open && close && (close - open - 1) >= 16) {
        strncpy(hex, open + 1, 16);
        hex[16] = '\0';
        return hex;
    }
    return NULL;
}

int main() {
    char infilename[256];
    char basename[256];
    char outfilename[256];

    printf("Enter file to clean (e.g. pcode.txt): ");
    fgets(infilename, sizeof(infilename), stdin);
    infilename[strcspn(infilename, "\n")] = '\0';

    strcpy(basename, infilename);
    char* dot = strrchr(basename, '.');
    if (dot && strcasecmp(dot, ".txt") == 0) *dot = '\0';
    snprintf(outfilename, sizeof(outfilename), "%s_clean.txt", basename);

    FILE* in  = fopen(infilename, "r");
    if (!in) { perror("Input"); return 1; }

    FILE* out = fopen(outfilename, "w");
    if (!out) { fclose(in); perror("Output"); return 1; }

    char line[MAX_LINE];
    char prev_hex[17] = {0};
    int removed = 0;
    int have_prev = 0;

    while (fgets(line, sizeof(line), in)) {
        const char* hex = get_hex(line);

        if (!hex) {
            fputs(line, out);
            have_prev = 0;
            continue;
        }

        if (!have_prev) {
            strcpy(prev_hex, hex);
            fputs(line, out);
            have_prev = 1;
            continue;
        }

        // Exception: if current hex is all zeros or all Fs → always keep
        int is_zero = strcmp(hex, "0000000000000000") == 0;
        int is_nan  = strcmp(hex, "FFFFFFFFFFFFFFFF") == 0;

        if (is_zero || is_nan) {
            fputs(line, out);
            strcpy(prev_hex, hex);   // still update prev so next one can be checked
            continue;
        }

        // Normal case: remove only if identical to previous kept line
        if (strcmp(hex, prev_hex) == 0) {
            removed++;
            continue;
        }

        strcpy(prev_hex, hex);
        fputs(line, out);
    }

    fclose(in);
    fclose(out);

    printf("Done → %s\n", outfilename);
    printf("Removed %d duplicate lines (real data only — zeros and NaNs preserved)\n", removed);
    return 0;
}