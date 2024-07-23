//
//  unicode.c
//  JustParsing
//
//  Created by Thomas Wetmore on 6/20/23.
//

#include "unicode.h"

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Function to check if a code point is a combining character
bool isCombiningCharacter(unsigned int codePoint) {
    // Check if the code point is in the range U+0300 to U+036F
    return (codePoint >= 0x0300 && codePoint <= 0x036F);
}

// Function to perform D-type Unicode normalization (NFD)
char* normalizeDType(const char* input) {
    // Get the length of the input string
    size_t inputLength = strlen(input);

    // Allocate memory for the normalized output
    char* normalized = (char*)malloc(sizeof(char) * (inputLength + 1));
    if (normalized == NULL) {
        // Handle memory allocation error
        return NULL;
    }

    // Keep track of the position in the normalized string
    size_t normalizedPos = 0;

    // Iterate over each character in the input string
    for (size_t i = 0; i < inputLength; i++) {
        // Get the current code point
        unsigned int codePoint = (unsigned char)input[i];

        // Check if the code point is a combining character
        if (isCombiningCharacter(codePoint)) {
            // Skip combining characters
            continue;
        }

        // Copy the current character to the normalized string
        normalized[normalizedPos++] = input[i];

        // Check if there are combining characters after the current character
        while ((i + 1) < inputLength && isCombiningCharacter((unsigned char)input[i + 1])) {
            // Copy the combining character to the normalized string
            normalized[normalizedPos++] = input[++i];
        }
    }

    // Null-terminate the normalized string
    normalized[normalizedPos] = '\0';

    return normalized;
}

//#include <stdlib.h>
//#include <string.h>
//#include <stdbool.h>
//
//// Function to check if a code point is a combining character
//bool isCombiningCharacter(unsigned char byte) {
//    // Check if the byte represents a UTF-8 combining character
//    return ((byte & 0xC0) == 0x80);
//}
//
//// Function to perform D-type Unicode normalization (NFD) on UTF-8 input
//char* normalizeDTypeUTF8(const char* input) {
//    // Get the length of the input string
//    size_t inputLength = strlen(input);
//
//    // Allocate memory for the normalized output
//    char* normalized = (char*)malloc(sizeof(char) * (inputLength + 1));
//    if (normalized == NULL) {
//        // Handle memory allocation error
//        return NULL;
//    }
//
//    // Keep track of the position in the normalized string
//    size_t normalizedPos = 0;
//
//    // Iterate over each byte in the input string
//    for (size_t i = 0; i < inputLength; ) {
//        // Get the current byte
//        unsigned char byte = input[i];
//
//        // Check if the byte represents a combining character
//        if (isCombiningCharacter(byte)) {
//            // Skip combining characters
//            i++;
//            continue;
//        }
//
//        // Copy the current character to the normalized string
//        normalized[normalizedPos++] = input[i++];
//
//        // Check if there are combining characters after the current character
//        while (i < inputLength && isCombiningCharacter(input[i])) {
//            // Copy the combining character to the normalized string
//            normalized[normalizedPos++] = input[i++];
//        }
//    }
//
//    // Null-terminate the normalized string
//    normalized[normalizedPos] = '\0';
//
//    return normalized;
//}
//#include <stdlib.h>
//#include <string.h>
//#include <stdbool.h>
//#include <stdint.h>
//
//// Function to check if a code point is a combining character
//bool isCombiningCharacter(uint8_t byte) {
//    // Check if the byte represents a UTF-8 combining character
//    return ((byte & 0xC0) == 0x80);
//}
//
//// Function to check if a code point is a leading byte of a UTF-8 character
//bool isLeadingByte(uint8_t byte) {
//    // Check if the byte represents the leading byte of a UTF-8 character
//    return ((byte & 0xC0) != 0x80);
//}
//
//// Function to perform D-type Unicode normalization (NFD) on UTF-8 input
//char* normalizeDTypeUTF8(const char* input) {
//    // Get the length of the input string
//    size_t inputLength = strlen(input);
//
//    // Allocate memory for the normalized output
//    char* normalized = (char*)malloc(sizeof(char) * (inputLength + 1));
//    if (normalized == NULL) {
//        // Handle memory allocation error
//        return NULL;
//    }
//
//    // Keep track of the position in the normalized string
//    size_t normalizedPos = 0;
//
//    // Iterate over each byte in the input string
//    for (size_t i = 0; i < inputLength; ) {
//        // Get the current byte
//        uint8_t byte = input[i];
//
//        // Check if the byte represents a combining character
//        if (isCombiningCharacter(byte)) {
//            // Skip combining characters
//            i++;
//            continue;
//        }
//
//        // Copy the current character to the normalized string
//        normalized[normalizedPos++] = input[i++];
//
//        // Check if there are combining characters after the current character
//        while (i < inputLength && isCombiningCharacter(input[i])) {
//            // Copy the combining character to the normalized string
//            normalized[normalizedPos++] = input[i++];
//        }
//
//        // Check if the current character needs to be decomposed
//        if (i < inputLength && isLeadingByte(input[i])) {
//            // Determine the length of the UTF-8 character
//            int utf8Length = 1;
//            uint8_t tempByte = input[i];
//            while ((tempByte & 0xC0) == 0x80) {
//                utf8Length++;
//                tempByte <<= 1;
//            }
//
//            // Verify if the character can be decomposed
//            if (utf8Length == 2 || utf8Length == 3) {
//                // Decompose the character into its base character and combining characters
//                for (int j = 0; j < utf8Length; j++) {
//                    normalized[normalizedPos++] = input[i++];
//                }
//            }
//        }
//    }
//
//    // Null-terminate the normalized string
//    normalized[normalizedPos] = '\0';
//
//    return normalized;
//}
