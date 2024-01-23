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

//from osdev wiki
void disable_cursor()
{
  outb(0x3D4, 0x0A);
  outb(0x3D5, 0x20);
}
void enable_cursor()
{
  outb(0x3D4, 0x0A);
  outb(0x3D5, (inb(0x3D5) & 0xC0) | 0x0F);
}
void update_cursor(int x, int y)
{
  uint16_t pos = y * VGA_WIDTH + x;

  outb(0x3D4, 0x0F);
  outb(0x3D5, (uint8_t) (pos & 0xFF));
  outb(0x3D4, 0x0E);
  outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
}
// 100 % stolen code pls work bc i need blinking to stop on lychee's computers :(
void disable_blinking() {
    // Read I/O Address 0x03DA to reset index/data flip-flop
    unsigned short dx = 0x03DA;
    inb(dx);

    // Write index 0x30 to 0x03C0 to set register index to 0x30
    dx = 0x03C0;
    outb(dx, 0x30);

    // Read from 0x03C1 to get register contents
    dx = 0x03C1;
    unsigned char al = inb(dx);

    // Unset Bit 3 to disable Blink
    al &= 0xF7;

    // Write to 0x03C0 to update register with changed value
    dx = 0x03C0;
    outb(dx, al);
}