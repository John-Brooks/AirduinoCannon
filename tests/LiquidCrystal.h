#pragma once

#include <memory>
#include <string>

class LiquidCrystal
{
public:
    LiquidCrystal(int rs, int enable, int d4, int d5, int d6, int d7);
    void begin(int cols, int rows);
    void setCursor(int col, int row);
    int print(const char* text);
    void clear();

    //The following functions are not supported by the arduino library and
    //may only be used in the testing environment.
    void PrintToConsole() const;
    std::string GetRow(int row) const;

private:
    std::unique_ptr<char[]> display_text;
    int cursor_row_pos = 0;
    int cursor_col_pos = 0;
    int allocated_rows = 0;
    int allocated_cols = 0;
};