#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>  // For tolower in case-insensitive check

#define MAX_PATH 256
#define MAX_LINE 256
#define MAX_HEX 17  // 16 digits + null

// Union to reinterpret uint64_t as double (big-endian hex to double)
typedef union {
    uint64_t u64;
    double dbl;
} hex_double_t;

// Function to convert hex string to uint64_t
uint64_t hex_to_u64(const char *hex) {
    uint64_t val = 0;
    int len = strlen(hex);
    if (len != 16) return 0;
    for (int i = 0; i < 16; i++) {
        char c = hex[i];
        val <<= 4;
        if (c >= '0' && c <= '9') val |= (c - '0');
        else if (c >= 'A' && c <= 'F') val |= (c - 'A' + 10);
        else if (c >= 'a' && c <= 'f') val |= (c - 'a' + 10);
        else return 0;  // Invalid
    }
    return val;
}

// Simple function to generate output filename from input (strip .txt case-insensitively, add .csv)
void generate_output_name(const char *input, char *output) {
    strncpy(output, input, MAX_PATH - 1);
    output[MAX_PATH - 1] = '\0';  // Safety null-term
    
    char *dot = strrchr(output, '.');
    if (dot) {
        // Case-insensitive check for ".txt"
        int is_txt = 1;
        char *ext = dot + 1;
        if (strlen(ext) == 3) {
            if (tolower(ext[0]) == 't' && tolower(ext[1]) == 'x' && tolower(ext[2]) == 't') {
                is_txt = 1;
            } else {
                is_txt = 0;
            }
        } else {
            is_txt = 0;
        }
        if (is_txt) {
            *dot = '\0';  // Chop off .txt
        }
    }
    strcat(output, ".csv");
}

// Function to decode and write to CSV
void process_file(const char *filename, const char *output) {
    FILE *in = fopen(filename, "r");
    if (!in) {
        fprintf(stderr, "Couldn't open %s—check the path?\n", filename);
        return;
    }

    FILE *out = fopen(output, "w");
    if (!out) {
        fprintf(stderr, "Couldn't open %s for writing.\n", output);
        fclose(in);
        return;
    }

    fprintf(out, "P_Number,Hex_Value,Decoded_Double\n");

    char line[MAX_LINE];
    int count = 0;
    while (fgets(line, sizeof(line), in)) {
        // Skip empty lines or non-G10 lines; handle leading %
        char *g10 = strstr(line, "G10");
        if (!g10) continue;
        
        // Trim leading junk like %
        char *start = g10 - 1;
        while (start > line && (*start == '%' || *start == ' ' || *start == '\t')) start--;
        if (start < line) start = line;
        memmove(line, start, strlen(start) + 1);
        
        // Trim trailing whitespace/newline
        char *end = line + strlen(line) - 1;
        while (end > line && (*end == ' ' || *end == '\n' || *end == '\r')) *end-- = '\0';

        if (strlen(line) < 10) continue;  // Too short

        // Extract P number: after "P" until before "("
        char *p_ptr = strstr(line, "P");
        if (!p_ptr) continue;
        p_ptr++;  // Skip 'P'
        char p_str[20] = {0};
        int i = 0;
        while (*p_ptr && *p_ptr != '(' && i < 19) {
            p_str[i++] = *p_ptr++;
        }
        int p_num = atoi(p_str);
        if (p_num == 0) continue;

        // Extract hex: from after "(" to before ")"
        char *open_paren = strchr(p_ptr, '(');
        if (!open_paren) continue;
        open_paren++;  // After (
        char *close_paren = strchr(open_paren, ')');
        if (!close_paren) continue;
        *close_paren = '\0';  // Temp null-terminate
        char hex[MAX_HEX] = {0};
        strncpy(hex, open_paren, sizeof(hex) - 1);
        
        // Trim trailing junk from hex
        char *hex_end = hex + strlen(hex) - 1;
        while (hex_end >= hex && (*hex_end == ' ' || *hex_end == '\t')) *hex_end-- = '\0';

        if (strlen(hex) == 16) {
            uint64_t u64 = hex_to_u64(hex);
            hex_double_t converter;
            converter.u64 = u64;
            double value = converter.dbl;

            fprintf(out, "%d,%s,%.10f\n", p_num, hex, value);
            count++;
            if (count % 100 == 0) printf(".");  // Progress dots
        }
    }

    fclose(in);
    fclose(out);
    printf("\nDecoded %d params from %s to %s\n", count, filename, output);
}

int main() {
    char input[MAX_PATH];
    char output[MAX_PATH];
    char user_override[MAX_PATH];

    printf("Enter the input filename (e.g., macro.txt): ");
    if (fgets(input, sizeof(input), stdin) == NULL) {
        fprintf(stderr, "Couldn't read input.\n");
        return 1;
    }
    // Trim newline from fgets
    input[strcspn(input, "\n")] = '\0';

    if (strlen(input) == 0) {
        fprintf(stderr, "No filename entered.\n");
        return 1;
    }

    generate_output_name(input, output);
    printf("Suggested output: %s\n", output);
    printf("Want a different output filename? (Press Enter for suggested, or type one): ");
    if (fgets(user_override, sizeof(user_override), stdin) != NULL) {
        // Trim newline
        user_override[strcspn(user_override, "\n")] = '\0';
        // Trim leading/trailing spaces
        char *start = user_override;
        while (*start == ' ' || *start == '\t') start++;
        char *end = start + strlen(start) - 1;
        while (end > start && (*end == ' ' || *end == '\t')) *end-- = '\0';
        
        if (strlen(start) > 0) {
            strncpy(output, start, MAX_PATH - 1);
            output[MAX_PATH - 1] = '\0';
        }
    }

    process_file(input, output);
    printf("All set—open %s in Excel.\n", output);
    return 0;
}