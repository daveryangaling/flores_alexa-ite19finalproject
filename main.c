#include <stdio.h> // Standard input and output library
#include <stdlib.h> // Standard library for functions like memory allocation and file handling
#include <string.h> // String manipulation functions
#include <math.h>   // For mathematical operations like abs()
#include <ctype.h>  // For character handling functions like isspace()
#define NULL_TERMINATOR '\0' // Define NULL_TERMINATOR as '\0' for readability

// Function declarations
int romanToDecimal(const char *roman); // Converts a Roman numeral to its decimal equivalent
void decimalToWords(int num);         // Converts a decimal number to its English word representation
void processLine(const char *line);   // Processes a line from the input file
void handleOperation(int num1, int num2, char op); // Handles arithmetic operations between two numbers
void trimLine(char *line);            // Removes leading and trailing spaces from a line

// Global variables
char *strNumToWords = NULL; // Pointer used to store the English word representation of a number
FILE *fileInput;  // Pointer to the input file
FILE *fileOutput; // Pointer to the output file

// Main function: Entry point of the program
int main() {
    // Open the input and output files
    fileInput = fopen("input.txt", "r");  // Open input.txt in read mode
    fileOutput = fopen("output.txt", "w"); // Open output.txt in write mode

    // Check if input file opened successfully
    if (fileInput == NULL) {
        perror("Error: Could not open input file."); // Print error message if file cannot be opened
        return EXIT_FAILURE; // Exit with failure status
    }

    // Check if output file opened successfully
    if (fileOutput == NULL) {
        perror("Error: Could not open output file."); // Print error message if file cannot be opened
        fclose(fileInput); // Close the input file before exiting
        return EXIT_FAILURE; // Exit with failure status
    }

    char line[256]; // Buffer to store a line of input
    // Read each line from the input file
    while (fgets(line, sizeof(line), fileInput) != NULL) {
        trimLine(line); // Remove leading and trailing spaces
        processLine(line); // Process the trimmed line
    }

    // Close the input and output files
    fclose(fileInput);
    fclose(fileOutput);

    printf("Processing complete. Check output.txt for results.\n"); // Indicate processing is complete
    return EXIT_SUCCESS; // Exit with success status
}

// Function to trim leading and trailing spaces from a line
void trimLine(char *line) {
    int end = strlen(line) - 1; // Find the last character's index
    // Remove trailing spaces
    while (end >= 0 && isspace(line[end])) {
        line[end--] = NULL_TERMINATOR; // Replace trailing spaces with NULL_TERMINATOR
    }

    int start = 0; // Start index for the line
    // Remove leading spaces
    while (line[start] && isspace(line[start])) {
        start++;
    }

    // Shift the string left to remove leading spaces
    if (start > 0) {
        memmove(line, line + start, strlen(line) - start + 1);
    }
}

// Function to process each line of the input file
void processLine(const char *line) {
    // Skip blank lines
    if (strlen(line) == 0) {
        return; // Do nothing for blank lines
    }

    char roman1[50], roman2[50], op; // Variables to store two Roman numerals and an operator
    // Parse the line into two Roman numerals and an operator
    if (sscanf(line, "%49s %c %49s", roman1, &op, roman2) != 3) {
        fprintf(fileOutput, "Invalid input\n"); // Write "Invalid input" to the output file if parsing fails
        return;
    }

    int num1 = romanToDecimal(roman1); // Convert the first Roman numeral to a decimal number
    int num2 = romanToDecimal(roman2); // Convert the second Roman numeral to a decimal number

    // Check if either Roman numeral is invalid
    if (num1 == -1 || num2 == -1) {
        fprintf(fileOutput, "Invalid Roman numeral(s)\n"); // Write error message if invalid Roman numeral
        return;
    }

    // Perform the operation and write the result to the output file
    handleOperation(num1, num2, op);
}

// Function to convert a Roman numeral to a decimal number
int romanToDecimal(const char *roman) {
    int total = 0;     // Total sum
    int current = 0;   // Current numeral's value
    int previous = 0;  // Previous numeral's value
    int repeatCount = 1; // Counter for consecutive identical numerals

    // Iterate through each character in the Roman numeral
    for (int i = 0; roman[i] != NULL_TERMINATOR; i++) {
        // Map Roman numeral to its decimal value
        switch (roman[i]) {
            case 'I': current = 1; break;
            case 'V': current = 5; break;
            case 'X': current = 10; break;
            case 'L': current = 50; break;
            case 'C': current = 100; break;
            case 'D': current = 500; break;
            case 'M': current = 1000; break;
            default: return -1; // Return -1 for invalid characters
        }

        // Check for more than three consecutive identical numerals
        if (i > 0 && roman[i] == roman[i - 1]) {
            repeatCount++;
            if (repeatCount > 3) {
                return -1; // Invalid Roman numeral
            }
        } else {
            repeatCount = 1; // Reset count for a new character
        }

        // Apply addition or subtraction rules
        if (current > previous) {
            total += (current - 2 * previous); // Subtract previous and add current
        } else {
            total += current;
        }

        previous = current; // Update previous numeral's value
    }

    return total; // Return the decimal equivalent
}

// Function to handle arithmetic operations
void handleOperation(int num1, int num2, char op) {
    int answer; // Variable to store the result

    // Perform the operation based on the operator
    switch (op) {
        case '+':
            answer = num1 + num2;
            break;
        case '-':
            answer = abs(num1 - num2); // Ensure the result is positive
            break;
        case '*':
            answer = num1 * num2;
            break;
        case '/':
            if (num2 != 0) answer = abs(num1 / num2); // Avoid division by zero
            else {
                fprintf(fileOutput, "Division by zero is undefined.\n");
                return;
            }
            break;
        default:
            fprintf(fileOutput, "Invalid operator: %c\n", op); // Write error message for invalid operator
            return;
    }

    decimalToWords(answer); // Convert the result to words
    fprintf(fileOutput, "%s\n", strNumToWords); // Write the result in words to the output file
    strNumToWords[0] = NULL_TERMINATOR; // Reset the word result
}

// Function to convert a decimal number to English words
void decimalToWords(int num) {
    const char *ones[] = {"", "One", "Two", "Three", "Four", "Five", "Six", "Seven", "Eight", "Nine"};
    const char *teens[] = {"", "Eleven", "Twelve", "Thirteen", "Fourteen", "Fifteen", "Sixteen", "Seventeen", "Eighteen", "Nineteen"};
    const char *tens[] = {"", "Ten", "Twenty", "Thirty", "Forty", "Fifty", "Sixty", "Seventy", "Eighty", "Ninety"};
    const char *thousands[] = {"", "Thousand", "Million", "Billion"};

    char result[1024] = ""; // Final result string
    int place = 0; // Place value for thousands, millions, etc.

    // Process the number in chunks of three digits
    while (num > 0) {
        int chunk = num % 1000; // Extract the last three digits
        if (chunk > 0) {
            char chunkStr[256] = "";
            int h = chunk / 100; // Hundreds place
            int t = (chunk % 100) / 10; // Tens place
            int o = chunk % 10; // Ones place

            if (h > 0) {
                strcat(chunkStr, ones[h]);
                strcat(chunkStr, " Hundred ");
            }
            if (t == 1 && o > 0) {
                strcat(chunkStr, teens[o]);
                strcat(chunkStr, " ");
            } else {
                if (t > 0) {
                    strcat(chunkStr, tens[t]);
                    strcat(chunkStr, " ");
                }
                if (o > 0) {
                    strcat(chunkStr, ones[o]);
                    strcat(chunkStr, " ");
                }
            }

            if (place > 0) {
                strcat(chunkStr, thousands[place]);
                strcat(chunkStr, " ");
            }
            strcat(chunkStr, result);
            strcpy(result, chunkStr);
        }
        num /= 1000; // Move to the next chunk
        place++;
    }

    strNumToWords = realloc(strNumToWords, strlen(result) + 1); // Allocate memory for the result
    strcpy(strNumToWords, result); // Copy the result to the global variable
}
