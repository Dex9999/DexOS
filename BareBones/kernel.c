#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* check if the compiler thinks you are targeting the wrong operating system. */
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

#if !defined(__i386__)
#error "This tutorial needs to be compiled with an ix86-elf compiler"
#endif

/* PS/2 Keyboard constants */
#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64

char key_buffer[256];  // Buffer to store pressed keys
int key_buffer_index = 0;  // Index to keep track of the next available position in the buffer

/* Hardware text mode color constants. */
enum vga_color {
    VGA_COLOR_BLACK = 0,
    VGA_COLOR_BLUE = 1,
    VGA_COLOR_GREEN = 2,
    VGA_COLOR_CYAN = 3,
    VGA_COLOR_RED = 4,
    VGA_COLOR_MAGENTA = 5,
    VGA_COLOR_BROWN = 6,
    VGA_COLOR_LIGHT_GREY = 7,
    VGA_COLOR_DARK_GREY = 8,
    VGA_COLOR_LIGHT_BLUE = 9,
    VGA_COLOR_LIGHT_GREEN = 10,
    VGA_COLOR_LIGHT_CYAN = 11,
    VGA_COLOR_LIGHT_RED = 12,
    VGA_COLOR_LIGHT_MAGENTA = 13,
    VGA_COLOR_LIGHT_BROWN = 14,
    VGA_COLOR_WHITE = 15,
};

static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) 
{
    return fg | bg << 4;
}

static inline uint16_t vga_entry(unsigned char uc, uint8_t color) 
{
    return (uint16_t) uc | (uint16_t) color << 8;
}

size_t strlen(const char* str) 
{
    size_t len = 0;
    while (str[len])
        len++;
    return len;
}

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;
size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer;

void terminal_initialize(void) 
{
    terminal_row = 0;
    terminal_column = 0;
    terminal_color = vga_entry_color(VGA_COLOR_GREEN, VGA_COLOR_DARK_GREY);
    terminal_buffer = (uint16_t*) 0xB8000;
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            terminal_buffer[index] = vga_entry(' ', terminal_color);
        }
    }
}

void terminal_setcolor(uint8_t color) 
{
    terminal_color = color;
}

void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) 
{
    const size_t index = y * VGA_WIDTH + x;
    terminal_buffer[index] = vga_entry(c, color);
}

void terminal_putchar(char c) 
{
    if (c == '\n') {
        terminal_column = 0;
        terminal_row++;
        return;
    }
    terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
    if (++terminal_column == VGA_WIDTH) {
        terminal_column = 0;
        if (++terminal_row == VGA_HEIGHT)
            terminal_row = 0;
    }
}

void terminal_write(const char* data, size_t size) 
{
    for (size_t i = 0; i < size; i++)
        terminal_putchar(data[i]);
}

void terminal_writestring(const char* data) 
{
    terminal_write(data, strlen(data));
}
// Function to convert an integer to a string
void itoa(int n, char str[], int base) {
    int i = 0;
    int isNegative = 0;

    // Handle 0 explicitly, otherwise empty string is printed
    if (n == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return;
    }

    // Handle negative numbers for bases other than 10
    if (n < 0 && base != 10) {
        isNegative = 1;
        n = -n;
    }

    // Process individual digits
    while (n != 0) {
        int rem = n % base;
        str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
        n = n / base;
    }

    // Add negative sign for negative numbers
    if (isNegative)
        str[i++] = '-';

    str[i] = '\0';

    // Reverse the string
    int start = 0;
    int end = i - 1;
    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
}

// Function to write an integer to the terminal
void terminal_writeint(int num, int base) {
    char num_str[32]; // Adjust the size based on the maximum expected digits
    itoa(num, num_str, base);
    terminal_writestring(num_str);
}

unsigned char inb(unsigned short port) {
    unsigned char result;
    asm volatile ("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

// Function to write a byte to an I/O port
void outb(unsigned short port, unsigned char data) {
    asm volatile ("outb %0, %1" :: "a"(data), "Nd"(port));
}

// Wait for the keyboard controller to be ready
void wait_kbd(void) {
    while ((inb(KEYBOARD_STATUS_PORT) & 0x02) != 0)
        ;
}

void init_kbd(void) {
    wait_kbd();
    outb(KEYBOARD_STATUS_PORT, 0xAE); // Enable the keyboard
}

char read_kbd(void) {
    wait_kbd();
    return inb(KEYBOARD_DATA_PORT);
}
bool newKey = true;
// Keycode reference table
char keycode_table[128] = {
    0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 0, 0,
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', 0, 'a', 's',
    'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0, '\\', 'z', 'x', 'c', 'v',
    'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' ', 0
};
#define KEY_BUFFER_SIZE 256

char key_buffer[KEY_BUFFER_SIZE];
size_t key_buffer_index = 0;

// Function to check if a key is already present in the buffer
bool is_key_in_buffer(char key) {
    for (size_t i = 0; i < key_buffer_index; ++i) {
        if (key_buffer[i] == key) {
            return true;
        }
    }
    return false;
}
void read_ps2_key(void) 
{
    char keycode = read_kbd();

    // Check if the key is released (ignore key release events)
    if (keycode < 0 || (keycode & 0x80) != 0)
        return;

    // Check if the key is already in the buffer
    if (is_key_in_buffer(keycode))
        return;

    // Add the key to the buffer
    key_buffer[key_buffer_index++] = keycode;
    
    // Process the key or perform any other actions as needed
    // For example, print the key to the screen:
    terminal_putchar(keycode_table[keycode]);

    // Handle Enter key press
    if (keycode == 0x1C) {
        // Do something when Enter is pressed
        terminal_writestring("ENTER");
    }

    // Reset the buffer if it's full
    if (key_buffer_index == KEY_BUFFER_SIZE)
        key_buffer_index = 0;
}


void kernel_main(void) 
{
    /* Initialize terminal interface */
    terminal_initialize();

    // Enable PS/2 keyboard
    init_kbd();

    terminal_writestring("\n");

   while (1) {
        read_ps2_key();
        // Add your main kernel logic here
    }
}
