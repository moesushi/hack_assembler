#include "splashkit.h"

string read_string(string prompt)
{
    string result;

    write(prompt);
    result = read_line();

    return result;
}

int read_integer(string prompt)
{
    string line;
    line = read_string(prompt);

    while (not is_integer(line))
    {
        write_line("Please enter a valid integer!");
        line = read_string(prompt);
    }

    return convert_to_integer(line);
}

int read_integer(string prompt, int min, int max)
{
    int input = read_integer(prompt);

    while (input < min or input > max)
    {
        write_line("Please enter a valid integer between " + std::to_string(min) + " and " + std::to_string(max));
        input = read_integer(prompt);
    }

    return input;
}

double read_double(string prompt)
{
    string line;
    line = read_string(prompt);

    while (not is_double(line))
    {
        write_line("Please enter a valid number!");
        line = read_string(prompt);
    }

    return convert_to_double(line);
}

double read_double_range(string prompt, double min, double max)
{
    double input = read_double(prompt);

    while (input < min or input > max)
    {
        write_line("Please enter a valid number between " + std::to_string(min) + " and " + std::to_string(max));
        input = read_double(prompt);
    }

    return input;
}

bool read_boolean(string prompt)
{
    while (true)
    {
        string input = read_string(prompt);
        input = trim(to_lowercase(input));
        if (input == "yes" or input == "y")
        {
            return true;
        }
        else if (input == "no" or input == "n")
        {
            return false;
        }
        else
        {
            write_line("Incorrect value for boolean input; input y[es] or n[o]");
        }
    }
}