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

#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64


/* VGA text mode color constants. */
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
    if(c == 0){
      // null char
      return;
    }
    if (c == '\n') {
        terminal_column = 0;
        terminal_row++;
        return;
    }
    if(c == '\t') {
        terminal_column += 4;
        return;
    }
    if(c == '\b'){
        if(terminal_column > 0){
          terminal_column--;
          terminal_putentryat(' ', terminal_color, terminal_column, terminal_row); 
          return;
        } else{return;}
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

void terminal_writeint(int num, int base) {
    char num_str[32]; 
    itoa(num, num_str, base);
    terminal_writestring(num_str);
}

//inb outb !!!!
unsigned char inb(unsigned short port) {
    unsigned char result;
    asm volatile ("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}
void outb(unsigned short port, unsigned char data) {
    asm volatile ("outb %0, %1" :: "a"(data), "Nd"(port));
}

void wait_kbd(void) {
    while ((inb(KEYBOARD_STATUS_PORT) & 0x02) != 0)
        ;
}

void init_kbd(void) {
    wait_kbd();
    outb(KEYBOARD_STATUS_PORT, 0xAE);
}

char read_kbd(void) {
    wait_kbd();
    return inb(KEYBOARD_DATA_PORT);
}
bool newKey = true;

char keycode_table[128] = {
    0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 0, 0,
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', 0, 'a', 's',
    'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0, '\\', 'z', 'x', 'c', 'v',
    'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' ', 0
};
char shift_keycode_table[128] = {
    0, 0, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', 0, 0,
    'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n', 0, 'A', 'S',
    'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '\"', '~', 0, '\|', 'Z', 'X', 'C', 'V',
    'B', 'N', 'M', '<', '>', '?', 0, '*', 0, ' ', 0
};
#define KEY_BUFFER_SIZE 256

char key_buffer[KEY_BUFFER_SIZE];
size_t key_buffer_index = 0;
bool shift = false;

void terminal_clear(void) 
{
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            terminal_buffer[index] = vga_entry(' ', terminal_color);
        }
    }

    // reset cursor position
    terminal_row = 0;
    terminal_column = 0;
}


char lastkeycode;
//super hacky way to not use interrupts, read every second
void read_ps2_key(void) 
{
    char keycode = read_kbd();
    
    if (keycode == lastkeycode)
        return;

    key_buffer[key_buffer_index++] = keycode;

   // don't print key releases
   if (!(keycode < 0 || (keycode & 0x80) != 0)){
     //deug mode for scancodes
    //  terminal_writeint(keycode, 16);
    //  lastkeycode=keycode;
    //  return;
     if(keycode == 1){
       //escape key
       terminal_clear();
     } else if(keycode == 0x0e){
       //backspace
       terminal_putchar('\b');
     } else if (keycode == 0x1C) {
       //enter key
       terminal_putchar('\n');
     } else if(keycode == 0x2A) {
       shift = !shift;
     } else if(keycode == 0x48){
       //up arrow
       terminal_row--;
     } else if(keycode == 0x50){
       //down arrow
       terminal_row++;
     } else if(keycode == 0x4B){
       //left arrow
       terminal_column--;
     } else if(keycode == 0x4D){
       //right arrow
       terminal_column++;
     } else if(shift){
      terminal_putchar(shift_keycode_table[keycode]);
    } else {
      terminal_putchar(keycode_table[keycode]);
    }
   }
   lastkeycode=keycode;
  
    // reset the buffer if full
    if (key_buffer_index == KEY_BUFFER_SIZE)
        key_buffer_index = 0;
}


//from osdev wiki
void disable_cursor()
{
  outb(0x3D4, 0x0A);
  outb(0x3D5, 0x20);
}
void update_cursor(int x, int y)
{
  uint16_t pos = y * VGA_WIDTH + x;

  outb(0x3D4, 0x0F);
  outb(0x3D5, (uint8_t) (pos & 0xFF));
  outb(0x3D4, 0x0E);
  outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
}

void kernel_main(void) 
{
    terminal_initialize();

    // send enable bytes for PS/2 keyboard
    init_kbd();
    //disable_cursor();

    terminal_writestring("Welcome to DexOS!!!!\n");

  //4 ever!
   while (1) {
        read_ps2_key();
        update_cursor(terminal_column, terminal_row);
    }
}