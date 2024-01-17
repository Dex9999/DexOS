#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "organized/vga.h"

#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64
int mode = 0;
//about a second, let user toggle later
int test_time = 15000000;



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
char caps_keycode_table[128] = {
    0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 0, 0,
    'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '[', ']', '\n', 0, 'A', 'S',
    'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', '\'', '`', 0, '\\', 'Z', 'X', 'C', 'V',
    'B', 'N', 'M', ',', '.', '/', 0, '*', 0, ' ', 0
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
bool caps = false;
bool ctrl = false;

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

// regular time function! :D nothing to see here
int wait(float seconds) {
    for (volatile int i = 0; i < seconds * test_time; i++){
        // wait
    }
    return 0;
}


const char *dude_left = "      ###     \n"
                        "[   ]#####    \n"
                        "  |  |- -|_   \n"
                        "  |/  \\_/  \\  \n"
                        "  ||_______|  \n" 
                        "   |___|___|  \n";

const char *dude_right = "      ###     \n"
                         "     #####[   ]\n"
                         "     |- -|_ |  \n"
                         "   /  \\_/  \\|  \n"
                         "   |_______||  \n" 
                         "   |___|___|  \n";


const char *dude_chop_left =
    "      ###         \n"
    "     #####         \n"
    "     |o o|_        \n"
    "   /  \\_/  \\     \n"
    "   |___   ----===\n"
    "   |___|___|     ";
const char *dude_chop_right =
    "        ###    \n"
    "       #####   \n"
    "      _|o o|   \n"
    "     /  \\_/  \\   \n"
    "===----   ___|   \n"
    "     |___|___|   ";

const char *dude_dead =
    "                  \n"
    " -------          \n"
    "/       \\        \n"
    "| R.I.P.|         \n"
    "|       |         \n"
    "| _  _ _|         ";
const char *trunk_1 = 
    "|   {  /    |\n"
    "| }   /     }\n"
    "{       }   |\n"
    "|        {  }\n"
    "| }    { \\  |\n"
    "{  |     /  }\n";
const char *trunk_2 = 
    "|           |\n"
    "|    {   |  |\n"
    "|        /  }\n"
    "{ } }   }   |\n"
    "| | }    }  |\n"
    "{           }";
const char *branch_l =
    "     o  o    \n"
    "   o0Oo  \\   \n"
    "  O0Oo  --\\  \n"
    "  oOOOo  /->-\n"
    "    o0O-/    \n"
    "             ";
const char *branch_r =
    "     oOo     \n"
    "     / oO    \n"
    "   /--00oO   \n"
    "--<   OOo    \n"
    "   \\oOO     ";

char lastkeycode;
unsigned short lfsr = 0xACE1u;
unsigned bit;

unsigned rand(){
    bit  = ((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5) ) & 1;
    return lfsr =  (lfsr >> 1) | (bit << 15);
}

//super hacky way to not use interrupts, read every second
void read_ps2_key(void) 
{
    char keycode = read_kbd();


    if (keycode == lastkeycode){
        return;
    }  
    

    key_buffer[key_buffer_index++] = keycode;

    //shift - the keyrelease           key release, right shift
    if ((keycode & 0x7F) == 0x2A && (keycode & 0x80) || (keycode & 0x7F) == 0x36 && (keycode & 0x80)) {
        shift = false;
    }

    if ((keycode & 0x7F) == 0x1D && (keycode & 0x80) || (keycode & 0x7F) == 0x5B && (keycode & 0x80)) {
        ctrl = false;
    }


   //debug mode for scancodes
   // 0x80 is key release
   if(mode == 2){
     //INCLUDE MAC CMD KEY
     if ((keycode & 0x7F) == 0x1D && (keycode & 0x80) || (keycode & 0x7F) == 0x5B && (keycode & 0x80)) {
         ctrl = false;
     }
     if(keycode == 0x5B || keycode == 0x1D){
       ctrl = true;
     }
     if(keycode & 0x80){
        terminal_writestring("0x");
        terminal_writeint((keycode & 0x7F), 16);
        terminal_writestring("  released ");
     } else{
        terminal_writestring("0x");
        terminal_writeint(keycode, 16);
        terminal_writestring("  pressed  ");
     }
     lastkeycode=keycode;
     terminal_column = 0;
     if(keycode == 0x20){
        if(ctrl){
          mode = 0;
          terminal_clear();
          terminal_writestring("Regular Mode!\n");
          ctrl = false;
        } 
      }
     return;
   }

    //tree dude mode 
   if(mode == 3){
     disable_cursor();
     terminal_row = VGA_HEIGHT/2;
     terminal_column = VGA_WIDTH/2-5;

     if(keycode == 0x4B){
       terminal_clear();
       terminal_writestring_at(dude_chop_left,VGA_WIDTH/2-5, VGA_HEIGHT/2);
       wait(1);
       terminal_clear();
       terminal_writestring_at(dude_left,VGA_WIDTH/2-5, VGA_HEIGHT/2);
     } else if(keycode == 0x4D){     
       terminal_clear();
       terminal_writestring_at(dude_chop_right,VGA_WIDTH/2-5, VGA_HEIGHT/2);
       wait(1);
       terminal_clear();
       terminal_writestring_at(dude_right,VGA_WIDTH/2-5, VGA_HEIGHT/2);

     } else if(keycode == 0x1D){     
       terminal_clear();
       terminal_writestring_at(dude_dead,VGA_WIDTH/2-5, VGA_HEIGHT/2);
     }
   }

   // no key releases
   if (!(keycode < 0 || (keycode & 0x80) != 0)){
    switch(keycode){
      case 1:
        //escape key
        terminal_clear();
        break;
      case 0x0e:
        //backspace
        terminal_putchar('\b');
        break;
      case 0x1C:
        //enter
        terminal_putchar('\n');
        break;
      case 0x2A:
        shift = true;
        break;
      case 0x36:
        //shift keydown only
        shift = true;
        break;
      case 0x3A:
        caps = !caps;
        break;
      case 0x5B:
        //cmd key on mac
        ctrl = true;
        break;
      case 0x1D:
        ctrl = true;
        break;
      case 0x0F:
        //tab
        terminal_putchar('\t');
        break;
      case 0x48:
        // make highlight / copy?
        //up arrow
        if (terminal_row > 0){
            terminal_row--;
        }
        break;
      case 0x50:
        //down arrow
        if (terminal_row < VGA_HEIGHT - 1){
            terminal_row++;
        }
        break;
      case 0x4B:
        //left arrow
        if (terminal_column > 0){
            terminal_column--;
        }
        break;
      case 0x4D:
        //right arrow
        if (terminal_column < VGA_WIDTH - 1){
            terminal_column++;
        }
        break;
      case 0x3B:
        terminal_writeint(rand(),10);
        terminal_putchar(' ');
        break;
      case 0x23:
        if(ctrl){
          terminal_clear();
          terminal_writestring("Ctrl-D for Debug Mode\nCtrl-T to play TreeDude\nF1 for random numbers");
          break;
        } 
      case 0x20:
        if(ctrl){
          mode = 2;
          terminal_clear();
          terminal_writestring("Debug Mode!\n");
          ctrl = false;
          break;
        } 
      case 0x14:
        if(ctrl){
          mode = 3;
          terminal_clear();
          terminal_writestring("TreeDude Mode!\n");
          break;
        } 
      case 0x19:
        if(ctrl){
          mode = 1;
          terminal_clear();
          terminal_writestring("Pong!\n");
          break;
        } 
      default:
        if(shift){
          terminal_putchar(shift_keycode_table[keycode]);
        } else if(caps){
          terminal_putchar(caps_keycode_table[keycode]);
        } else {
          terminal_putchar(keycode_table[keycode]);
        }
    }
   }
   lastkeycode=keycode;

    // reset the buffer if full
    if (key_buffer_index == KEY_BUFFER_SIZE)
        key_buffer_index = 0;
}

int ball_x = VGA_WIDTH/2;
int ball_y = VGA_HEIGHT/2;
int velocity_x = 1;
int velocity_y = 1;\
int paddle_x = VGA_WIDTH/2;
int paddle_y = VGA_HEIGHT-1;
int paddle_2_x = VGA_WIDTH/2;
int paddle_2_y = 0;
char ball = 'O';
bool paddle_1_left = false;
bool paddle_1_right = false;
bool paddle_2_left = false;
bool paddle_2_right = false;
int score[] = {0,0};
void input_wait(float seconds, bool *paddle_1_left, bool *paddle_1_right, bool *paddle_2_left, bool *paddle_2_right) {
    for (volatile int i = 0; i < seconds * (test_time/2); i++) {
        char keycode = read_kbd();
        if (keycode == lastkeycode) {
            continue;
        }
        if (keycode == 0x4b) {
            *paddle_1_left = true;
        } else if (keycode == 0x4d) {
            *paddle_1_right = true;
        }
        if (keycode & 0x80) {
            if ((keycode & 0x7F) == 0x4B) {
                *paddle_1_left = false;
            } else if ((keycode & 0x7F) == 0x4D) {
                *paddle_1_right = false;
            }
        }

        if (*paddle_1_left) {
            paddle_x--;
        } else if (*paddle_1_right) {
            paddle_x++;
        }
        lastkeycode = keycode;
    }
}

void kernel_main(void) 
{
    terminal_initialize();

    // send enable bytes for PS/2 keyboard
    init_kbd();
    //disable_cursor();

    // make some computers play nice vvv
    disable_blinking();

    terminal_writestring("Welcome to DexOS!!!! (ctrl-h for help)\n");


  //4 ever!
   while (1) {
        if(mode == 0 || mode == 2 || mode == 3){
          read_ps2_key();
          update_cursor(terminal_column, terminal_row);
        } else if(mode == 1){
          //test timer?
          
          
          terminal_clear();
          
          terminal_writestring_at("------", VGA_WIDTH/2-5, 0); 
          terminal_writestring_at("------", paddle_x, paddle_y);

          terminal_column = 0;
          terminal_row = 0;
          terminal_writeint(ball_x,10);
          terminal_writestring(" : ");
          terminal_writeint(paddle_x,10);
          
          //balls
          terminal_putentryat(ball, terminal_color, ball_x, ball_y);
          ball_x += velocity_x;
          ball_y += velocity_y;
          if(ball_x >= VGA_WIDTH-1 || ball_x <= 0){
            velocity_x = -velocity_x;
          }

          //collison code needs to work <3
          if(ball_y >= VGA_HEIGHT || ball_y <= 0){
            velocity_y = -velocity_y;
            if(ball_y < 0){
              if(ball_x >= paddle_2_x && ball_x <= paddle_2_x+6){
                velocity_y = -velocity_y;
              } else {
                score[1]++;
                ball_x = VGA_WIDTH/2;
                ball_y = VGA_HEIGHT/2;
              }
            } else if (ball_y >= VGA_HEIGHT-1){
              if(ball_x >= paddle_x && ball_x <= paddle_x+6){
                velocity_y = -velocity_y;
              } else {
                score[0]++;
                ball_x = VGA_WIDTH/2;
                ball_y = VGA_HEIGHT/2;
              }
            }
            
          }
          
          //input and define a clear(-ish) framerate thingy
          input_wait(0.01, &paddle_1_left, &paddle_1_right, &paddle_2_left, &paddle_2_right);
        } else{
          terminal_writestring("WTF DID YOU DO");
        }
    }
}