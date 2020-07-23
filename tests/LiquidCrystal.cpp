#include "LiquidCrystal.h"
#include <cstring>
#include <stdexcept>

LiquidCrystal::LiquidCrystal(int rs, int enable, int d4, int d5, int d6, int d7)
{

}
void LiquidCrystal::clear()
{
   memset(display_text.get(), ' ', allocated_cols*allocated_rows);
   display_text[allocated_cols*allocated_rows] = '\0';
}

void LiquidCrystal::begin(int cols, int rows)
{
    display_text = std::make_unique<char[]>(cols*rows + 1);
    allocated_cols = cols;
    allocated_rows = rows;
    clear();
}

void LiquidCrystal::setCursor(int col, int row)
{
    if(col >= allocated_cols || row >= allocated_rows)
        throw std::out_of_range("Invalid LCD cursor position");

    cursor_col_pos = col;
    cursor_row_pos = row;
}
int LiquidCrystal::print(const char* text)
{
    int string_length = strlen(text);
    if(cursor_col_pos + string_length > allocated_cols)
        throw std::overflow_error("Text overflow in LCD display");

    size_t cursor_index = (allocated_cols * cursor_row_pos) + cursor_col_pos;
    memcpy(&display_text[cursor_index], text, string_length);

    return string_length;
}

void PrintHorizontalLine(int cols)
{
    printf("-");
    for(int i = 0; i < cols; i++)
        printf("-");
    printf("-");
    printf("\n");
}

void LiquidCrystal::PrintToConsole() const
{
    PrintHorizontalLine(allocated_cols);

    for(int i = 0; i < allocated_rows; i++)
        printf("|%s|\n", GetRow(i).c_str());

    PrintHorizontalLine(allocated_cols);
}

std::string LiquidCrystal::GetRow(int row) const
{
    auto row_buffer = std::make_unique<char[]>(allocated_cols + 1);
    size_t text_index = (allocated_cols * row);
    memcpy(row_buffer.get(), &display_text[text_index], allocated_cols);
    row_buffer[allocated_cols] = '\0';
    return std::string(row_buffer.get());
}