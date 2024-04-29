#include <stdio.h>
#include <stdint.h>

typedef uint32_t CODEPOINT; // Type for storing a Unicode code point (up to 6 hex digits)
typedef uint8_t UTF8;       // Type for UTF-8 encoding units (8-bit)
typedef uint16_t UTF16;     // Type for UTF-16 encoding units (16-bit)

// Function prototypes
int convert_codepoint_to_utf8(CODEPOINT codepoint, UTF8 *output);
int convert_codepoint_to_utf16(CODEPOINT codepoint, UTF16 *output);
int convert_utf8_to_codepoint(const UTF8 *input, CODEPOINT *codepoint);
int convert_utf16_to_codepoint(const UTF16 *input, CODEPOINT *codepoint);
int is_valid_utf8(const UTF8 *input, size_t length);

// Conversion implementations
int convert_codepoint_to_utf8(CODEPOINT codepoint, UTF8 *output)
{
    if (!output)
        return -1; // Error: Output pointer is NULL

    if (codepoint <= 0x7F)
    {
        output[0] = (UTF8)codepoint;
        return 1;
    }
    else if (codepoint <= 0x7FF)
    {
        output[0] = (UTF8)(0xC0 | (codepoint >> 6));
        output[1] = (UTF8)(0x80 | (codepoint & 0x3F));
        return 2;
    }
    else if (codepoint <= 0xFFFF)
    {
        output[0] = (UTF8)(0xE0 | (codepoint >> 12));
        output[1] = (UTF8)(0x80 | ((codepoint >> 6) & 0x3F));
        output[2] = (UTF8)(0x80 | (codepoint & 0x3F));
        return 3;
    }
    else if (codepoint <= 0x10FFFF)
    {
        output[0] = (UTF8)(0xF0 | (codepoint >> 18));
        output[1] = (UTF8)(0x80 | ((codepoint >> 12) & 0x3F));
        output[2] = (UTF8)(0x80 | ((codepoint >> 6) & 0x3F));
        output[3] = (UTF8)(0x80 | (codepoint & 0x3F));
        return 4;
    }
    return 0; // Error: Invalid code point
}

int convert_codepoint_to_utf16(CODEPOINT codepoint, UTF16 *output)
{
    if (!output)
        return -1; // Error: Output pointer is NULL

    if (codepoint <= 0xFFFF)
    {
        // Directly encode codepoints that do not require surrogate pairs
        output[0] = (UTF16)codepoint;
        return 1;
    }
    else if (codepoint <= 0x10FFFF)
    {
        // Encode codepoints that require surrogate pairs
        codepoint -= 0x10000;
        output[0] = (UTF16)(0xD800 | (codepoint >> 10));
        output[1] = (UTF16)(0xDC00 | (codepoint & 0x3FF));
        return 2;
    }
    return 0; // Error: Invalid code point
}

int convert_utf8_to_codepoint(const UTF8 *input, CODEPOINT *codepoint)
{
    if (!input || !codepoint)
        return -1; // Error: Input or output pointer is NULL

    const unsigned char *byte = input;
    int num_bytes = 0;
    *codepoint = 0;

    // Determine the number of bytes in the UTF-8 sequence based on the first byte
    if (*byte < 0x80)
    {
        *codepoint = *byte;
        return 1;
    }
    else if ((*byte & 0xE0) == 0xC0)
    {
        *codepoint = *byte & 0x1F;
        num_bytes = 1;
    }
    else if ((*byte & 0xF0) == 0xE0)
    {
        *codepoint = *byte & 0x0F;
        num_bytes = 2;
    }
    else if ((*byte & 0xF8) == 0xF0)
    {
        *codepoint = *byte & 0x07;
        num_bytes = 3;
    }
    else
    {
        return 0; // Error: Invalid UTF-8 start byte
    }

    for (int i = 1; i <= num_bytes; i++)
    {
        if ((input[i] & 0xC0) != 0x80)
            return 0; // Error: Invalid UTF-8 continuation byte
        *codepoint = (*codepoint << 6) | (input[i] & 0x3F);
    }

    return num_bytes + 1;
}

int convert_utf16_to_codepoint(const UTF16 *input, CODEPOINT *codepoint)
{
    if (!input || !codepoint)
        return -1; // Error: Input or output pointer is NULL

    if (*input >= 0xD800 && *input <= 0xDBFF)
    {
        // Check for a valid low surrogate following a high surrogate
        if (*(input + 1) >= 0xDC00 && *(input + 1) <= 0xDFFF)
        {
            *codepoint = ((*input - 0xD800) << 10) | (*(input + 1) - 0xDC00) + 0x10000;
            return 2;
        }
        return 0; // Error: Missing low surrogate after high surrogate
    }
    else if (*input >= 0xDC00 && *input <= 0xDFFF)
    {
        return 0; // Error: Stray low surrogate without preceding high surrogate
    }

    // Direct conversion for non-surrogate values
    *codepoint = *input;
    return 1;
}

int is_valid_utf8(const UTF8 *input, size_t length)
{
    size_t i = 0;
    while (i < length)
    {
        if (input[i] < 0x80)
        {
            // 0xxxxxxx - 1 byte sequence
            i++;
        }
        else if ((input[i] & 0xE0) == 0xC0)
        {
            // 110xxxxx 10xxxxxx - 2 byte sequence
            if (i + 1 >= length || (input[i + 1] & 0xC0) != 0x80)
                return 0; // Invalid continuation byte
            i += 2;
        }
        else if ((input[i] & 0xF0) == 0xE0)
        {
            // 1110xxxx 10xxxxxx 10xxxxxx - 3 byte sequence
            if (i + 2 >= length || (input[i + 1] & 0xC0) != 0x80 || (input[i + 2] & 0xC0) != 0x80)
                return 0; // Invalid continuation bytes
            i += 3;
        }
        else if ((input[i] & 0xF8) == 0xF0)
        {
            // 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx - 4 byte sequence
            if (i + 3 >= length || (input[i + 1] & 0xC0) != 0x80 || (input[i + 2] & 0xC0) != 0x80 || (input[i + 3] & 0xC0) != 0x80)
                return 0; // Invalid continuation bytes
            i += 4;
        }
        else
        {
            return 0; // Invalid UTF-8 start byte
        }
    }
    return 1; // Valid UTF-8
}

// Main program for testing
int main()
{
    // Test cases as provided
    CODEPOINT codepoints[] = {0x007A, 0x00A3, 0x20AC, 0x1F600}; // Unicode codepoints for 'z', '£', '€', '😀'
    UTF8 utf8_output[4][4];                                     // UTF-8 output buffer
    UTF16 utf16_output[4][2];                                   // UTF-16 output buffer
    CODEPOINT cp;
    int utf8_len, utf16_len;

    // Testing conversions between code points and UTF-8/UTF-16
    // Convert each code point to UTF-8 and UTF-16, then convert back
    for (int i = 0; i < 4; i++)
    {
        utf8_len = convert_codepoint_to_utf8(codepoints[i], utf8_output[i]);
        utf16_len = convert_codepoint_to_utf16(codepoints[i], utf16_output[i]);

        printf("Code point (HEX): U+%06X\n", codepoints[i]);
        printf("UTF-8 (HEX): ");
        for (int j = 0; j < utf8_len; j++)
        {
            printf("%02X ", utf8_output[i][j]);
        }
        printf("\nUTF-16 (HEX): ");
        for (int j = 0; j < utf16_len; j++)
        {
            printf("%04X ", utf16_output[i][j]);
        }
        printf("\n");

        convert_utf8_to_codepoint(utf8_output[i], &cp);
        printf("Decoded from UTF-8: U+%06X\n", cp);
        convert_utf16_to_codepoint(utf16_output[i], &cp);
        printf("Decoded from UTF-16: U+%06X\n", cp);
        printf("\n");
    }

    // Additional UTF-8 validity tests
    UTF8 test1[] = {0xF0, 0x9F, 0x92, 0xA9}; // Valid UTF-8 sequence for '💩'
    UTF8 test2[] = {0xF0, 0x28, 0x8C, 0x28}; // Invalid UTF-8 sequence

    printf("UTF-8 Validity Test 1: %s\n", is_valid_utf8(test1, sizeof(test1)) ? "valid" : "invalid");
    printf("UTF-8 Validity Test 2: %s\n", is_valid_utf8(test2, sizeof(test2)) ? "valid" : "invalid");

    return 0;
}
