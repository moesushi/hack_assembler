#ifndef TERMINAL_USER_INPUT
#define TERMINAL_USER_INPUT

#include "string"
using std::string;

/**
 * Prompts the user for an input and reads in their input from the terminal.
 * The input will be returned as a string.
 *
 * @param prompt The message displayed to the user
 * @returns The text the user entered
 */
string read_string(string prompt);

/**
 * Prompts the user for an input and reads in their input from the terminal.
 * The input will be converted into an integer and returned as an integer.
 *
 * @param prompt The message displayed to the user
 * @returns The integer the user entered
 */
int read_integer(string prompt);

/**
 * Prompts the user for a whole number between a range and reads in their input from the terminal.
 * The input will be converted into an integer and returned as an integer.
 *
 * @param prompt The message displayed to the user
 * @param min The minimum number the user can enter 
 * @param max The maximum number the user can enter
 * 
 * @returns The integer the user entered
 */
int read_integer(string prompt, int min, int max);

/**
 * Prompts the user for an input and reads in their input from the terminal.
 * The input will be converted into a double and returned as a double.
 *
 * @param prompt The message displayed to the user
 * @returns The double the user entered
 */
double read_double(string prompt);

/**
 * Prompts the user for a number between a range and reads in their input from the terminal.
 * The input will be converted into a double and returned as a double.
 *
 * @param prompt The message displayed to the user
 * @param min The minimum number the user can enter 
 * @param max The maximum number the user can enter
 * 
 * @returns The double the user entered
 */
double read_double_range(string prompt, double min, double max);

/**
 * Prompts the user for a boolean and reads in their input from the terminal.
 * The input will be converted into a boolean and returned as a boolean.
 *
 * @param prompt The message displayed to the user
 * @returns The boolean the user entered
 */
bool read_boolean(string prompt);

#endif