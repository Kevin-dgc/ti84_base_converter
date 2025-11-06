#include <ti/screen.h>
#include <ti/getkey.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

void long_long_to_binary(unsigned long long n, char *out_str) {
    int str_index = 0;
    for (int i = 63; i >= 0; i--) {
        if (i % 8 == 7 && i != 63) {
            out_str[str_index++] = ',';
            out_str[str_index++] = ' ';
        }
        
        if ((n >> i) & 1) {
            out_str[str_index++] = '1';
        } else {
            out_str[str_index++] = '0';
        }
    }
    out_str[str_index] = '\0';
}

void long_long_to_hex(unsigned long long n, char *out_str) {
    int str_index = 0;
    out_str[str_index++] = '0';
    out_str[str_index++] = 'x';

    for (int i = 7; i >= 0; i--) {
        out_str[str_index++] = ' ';

        unsigned char byte = (n >> (i * 8)) & 0xFF;

        unsigned int nibble_hi = (byte >> 4) & 0xF;
        if (nibble_hi < 10) {
            out_str[str_index++] = nibble_hi + '0';
        } else {
            out_str[str_index++] = (nibble_hi - 10) + 'A';
        }

        unsigned int nibble_lo = byte & 0xF;
        if (nibble_lo < 10) {
            out_str[str_index++] = nibble_lo + '0';
        } else {
            out_str[str_index++] = (nibble_lo - 10) + 'A';
        }
    }
    out_str[str_index] = '\0';
}

unsigned long long binary_to_long_long(const char *bin_str) {
    unsigned long long result = 0;
    
    for (int i = 0; bin_str[i] != '\0'; i++) {
        if (bin_str[i] == '1') {
            result = (result << 1) | 1;
        } else if (bin_str[i] == '0') {
            result = result << 1;
        }
    }
    
    return result;
}

unsigned long long hex_to_long_long(const char *hex_str) {
    unsigned long long result = 0;
    int i = 0;
    
    if (hex_str[0] == '0' && (hex_str[1] == 'x' || hex_str[1] == 'X')) {
        i = 2;
    }
    
    for (; hex_str[i] != '\0'; i++) {
        char c = hex_str[i];
        
        if (c == ' ') {
            continue;
        }
        
        unsigned int digit;
        if (c >= '0' && c <= '9') {
            digit = c - '0';
        } else if (c >= 'A' && c <= 'F') {
            digit = (c - 'A') + 10;
        } else if (c >= 'a' && c <= 'f') {
            digit = (c - 'a') + 10;
        } else {
            continue;
        }
        
        result = (result << 4) | digit;
    }
    
    return result;
}

// Custom function to format binary with specified bit width (comma every 4 bits)
void format_binary_custom(unsigned long long n, char *out_str, int bits) {
    int str_index = 0;
    
    for (int i = bits - 1; i >= 0; i--) {
        if (i % 4 == 3 && i != bits - 1) {
            out_str[str_index++] = ',';
            out_str[str_index++] = ' ';
        }
        
        if ((n >> i) & 1) {
            out_str[str_index++] = '1';
        } else {
            out_str[str_index++] = '0';
        }
    }
    out_str[str_index] = '\0';
}

// Custom function to format hex with specified number of hex digits
void format_hex_custom(unsigned long long n, char *out_str, int hex_digits) {
    int str_index = 0;
    out_str[str_index++] = '0';
    out_str[str_index++] = 'x';
    
    for (int i = hex_digits - 1; i >= 0; i--) {
        unsigned int nibble = (n >> (i * 4)) & 0xF;
        if (nibble < 10) {
            out_str[str_index++] = nibble + '0';
        } else {
            out_str[str_index++] = (nibble - 10) + 'A';
        }
    }
    out_str[str_index] = '\0';
}

void dec_fun(){
    char input_buffer[21];
    char hex_str[40];
    char bin_str[100];
    char bit_size_str[30];
    
    unsigned long long dec;
    os_ClrHome();

    while(true) {
        
        os_GetStringInput("Enter number:", input_buffer, 21);
        if (input_buffer[0] == '\0') {
            break; 
        }
        
        bool is_negative = false;
        if (input_buffer[0] == '-') {
            is_negative = true;
            unsigned long long magnitude = strtoull(input_buffer + 1, NULL, 10);
            dec = ~magnitude + 1;
        } else {
            dec = strtoull(input_buffer, NULL, 10);
        }

        // Determine bit size needed
        int bits = 64;
        if (!is_negative) {
            if (dec <= 0xFF) bits = 8;
            else if (dec <= 0xFFFF) bits = 16;
            else if (dec <= 0xFFFFFFFF) bits = 32;
        } else {
            // For negative, check if it fits in smaller signed ranges
            long long signed_dec = (long long)dec;
            if (signed_dec >= -128 && signed_dec <= 127) bits = 8;
            else if (signed_dec >= -32768 && signed_dec <= 32767) bits = 16;
            else if (signed_dec >= -2147483648LL && signed_dec <= 2147483647LL) bits = 32;
        }
        
        // Mask to the appropriate bit size
        unsigned long long mask = (bits == 64) ? ~0ULL : ((1ULL << bits) - 1);
        dec = dec & mask;

        int hex_digits = bits / 4;
        format_hex_custom(dec, hex_str, hex_digits);
        format_binary_custom(dec, bin_str, bits);

        sprintf(bit_size_str, "Bit size: %d", bits);
        
        os_NewLine();
        os_PutStrFull(bit_size_str);
        os_NewLine();  
        os_NewLine();        
        os_PutStrFull("Hex: ");
        os_PutStrFull(hex_str);
        os_NewLine();
        os_NewLine();  
        os_PutStrFull("Bin: ");
        os_PutStrFull(bin_str);
        while(true) {
            uint16_t key = os_GetKey();
            if (key == 0x09) {
                return;
            }
            if (key != 0) {
                os_ClrHome();
                break;
            }
        }
    }
}

void hex_fun(){
    char input_buffer[21];
    char sign_buffer[10];
    char dec_str[25];
    char hex_str[40];
    char bin_str[100];
    char bit_size_str[30];
    
    unsigned long long n;
    os_ClrHome();
    while(true) {
        
        os_GetStringInput("Enter hex number:", input_buffer, 21);
        if (input_buffer[0] == '\0') {
            break; 
        }
        
        os_NewLine();
        os_GetStringInput("Unsigned(0) Signed(1):", sign_buffer, 10);
        
        n = hex_to_long_long(input_buffer);
        
        // Count actual hex digits in input (each hex digit = 4 bits)
        int hex_digits = 0;
        for (int i = 0; input_buffer[i] != '\0'; i++) {
            char c = input_buffer[i];
            if ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f')) {
                hex_digits++;
            }
        }
        
        // Actual bit size from input
        int actual_bits = hex_digits * 4;
        
        // Display bit size (rounded up)
        int display_bits = actual_bits;
        if (display_bits <= 8) display_bits = 8;
        else if (display_bits <= 16) display_bits = 16;
        else if (display_bits <= 32) display_bits = 32;
        else display_bits = 64;
        
        // Mask to actual input size
        unsigned long long actual_mask = (actual_bits == 64) ? ~0ULL : ((1ULL << actual_bits) - 1);
        n = n & actual_mask;
        
        // If signed, check sign bit at ACTUAL input position and sign-extend
        if (sign_buffer[0] == '1') {
            unsigned long long sign_bit = 1ULL << (actual_bits - 1);
            if (n & sign_bit) {
                // MSB of actual input is set - sign extend from actual_bits to 64 bits
                unsigned long long extend_mask = ~actual_mask;
                n |= extend_mask;
            }
        }
        
        // For displaying binary and hex, mask to display size
        unsigned long long display_mask = (display_bits == 64) ? ~0ULL : ((1ULL << display_bits) - 1);
        unsigned long long display_n = n & display_mask;
        
        // Format decimal (using full 64-bit value for proper signed display)
        if (sign_buffer[0] == '1') {
            long long signed_n = (long long)n;
            sprintf(dec_str, "%lld", signed_n);
        } else {
            sprintf(dec_str, "%llu", n);
        }
        
        int display_hex_digits = display_bits / 4;
        format_binary_custom(display_n, bin_str, display_bits);
        format_hex_custom(display_n, hex_str, display_hex_digits);
        sprintf(bit_size_str, "Bit size: %d", display_bits);
        
        os_NewLine();
        os_PutStrFull(bit_size_str);
        os_NewLine();  
        os_NewLine();        
        os_PutStrFull("Dec: ");
        os_PutStrFull(dec_str);
        os_NewLine();
        os_NewLine();  
        os_PutStrFull("Hex: ");
        os_PutStrFull(hex_str);
        os_NewLine();
        os_NewLine();  
        os_PutStrFull("Bin: ");
        os_PutStrFull(bin_str);
        while(true) {
            uint16_t key = os_GetKey();
            if (key == 0x09) {
                return; 
            }
            if (key != 0) {
                os_ClrHome();
                break;
            }
        }
    }
}

void bin_fun(){
    char input_buffer[100];
    char sign_buffer[10];
    char hex_str[40];
    char dec_str[25];
    char bin_str[100];
    char bit_size_str[30];
    
    unsigned long long n;
    os_ClrHome();
    while(true) {
        
        os_GetStringInput("Enter binary number:", input_buffer, 100);
        if (input_buffer[0] == '\0') {
            break; 
        }
        
        os_NewLine();
        os_GetStringInput("Unsigned(0) Signed(1):", sign_buffer, 10);
        
        n = binary_to_long_long(input_buffer);
        
        // Count actual bits in input
        int actual_bits = 0;
        for (int i = 0; input_buffer[i] != '\0'; i++) {
            if (input_buffer[i] == '0' || input_buffer[i] == '1') {
                actual_bits++;
            }
        }
        
        // Display bit size (rounded up)
        int display_bits = actual_bits;
        if (display_bits <= 8) display_bits = 8;
        else if (display_bits <= 16) display_bits = 16;
        else if (display_bits <= 32) display_bits = 32;
        else display_bits = 64;
        
        // Mask to actual input size
        unsigned long long actual_mask = (actual_bits == 64) ? ~0ULL : ((1ULL << actual_bits) - 1);
        n = n & actual_mask;
        
        // If signed, check sign bit at ACTUAL input position and sign-extend
        if (sign_buffer[0] == '1') {
            unsigned long long sign_bit = 1ULL << (actual_bits - 1);
            if (n & sign_bit) {
                // MSB of actual input is set - sign extend from actual_bits to 64 bits
                unsigned long long extend_mask = ~actual_mask;
                n |= extend_mask;
            }
        }
        
        // For displaying binary and hex, mask to display size
        unsigned long long display_mask = (display_bits == 64) ? ~0ULL : ((1ULL << display_bits) - 1);
        unsigned long long display_n = n & display_mask;
        
        // Format decimal (using full 64-bit value for proper signed display)
        if (sign_buffer[0] == '1') {
            long long signed_n = (long long)n;
            sprintf(dec_str, "%lld", signed_n);
        } else {
            sprintf(dec_str, "%llu", n);
        }
        
        int display_hex_digits = display_bits / 4;
        format_hex_custom(display_n, hex_str, display_hex_digits);
        format_binary_custom(display_n, bin_str, display_bits);
        sprintf(bit_size_str, "Bit size: %d", display_bits);
        
        os_NewLine();
        os_PutStrFull(bit_size_str);
        os_NewLine();
        os_NewLine();
        os_PutStrFull("Bin: ");
        os_PutStrFull(bin_str);
        os_NewLine();
        os_NewLine();        
        os_PutStrFull("Dec: ");
        os_PutStrFull(dec_str);
        os_NewLine();
        os_NewLine();  
        os_PutStrFull("Hex: ");
        os_PutStrFull(hex_str);
        while(true) {
            uint16_t key = os_GetKey();
            if (key == 0x09) {
                return; 
            }
            if (key != 0) {
                os_ClrHome();
                break;
            }
        }
    }
}

int main(void) {
    while(true){
        char choice[10];
        os_ClrHome();
        os_PutStrFull("Decimal: 1");
        os_NewLine();        

        os_PutStrFull("Hex: 2");
        os_NewLine();  
        
        os_PutStrFull("Binary: 3");
        os_NewLine();  

        os_GetStringInput("", choice, 10);

        if(choice[0] == '1'){
            dec_fun();
        }else if(choice[0] == '2'){
            hex_fun();
        }
        else if(choice[0] == '3'){
            bin_fun();
        }
        else{
            os_NewLine();  
            os_PutStrFull("Invalid Input");
            return 0;
        }
    }
    return 0;
}
