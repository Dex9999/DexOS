#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

//file splitting is on hold till all the code works

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

//after ?
#include "organized/vga.h"

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
      case 0x1F:
        if(ctrl){
          terminal_clear();
          mode = 4;
          break;
        } 
      case 0x23:
        if(ctrl){
          terminal_clear();
          terminal_writestring("Ctrl-D for Debug Mode\nCtrl-P to play Pong\nF1 for random numbers");
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
          disable_cursor();
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
bool countup = false;
bool countdown = false;
void settings_input_wait(float seconds) {
    //for (volatile int elapsed_time = 0; elapsed_time < test_time*seconds; elapsed_time++) {
        if (countup) {
            test_time += 100;
        }
        if (countdown) {
            test_time -= 100;
        }

        char keycode = read_kbd();

        if (keycode == lastkeycode) {
            return;
        }

        if (keycode == 0x1D) {
            ctrl = true;
        }

        if (keycode == 0x48) {
            countup = true;
            test_time += 100;
        }

        if (keycode == 0x50) {
            countdown = true;
            test_time -= 100;
        }

        if (keycode == 0x17) {
          //i
            countup = true;
            test_time += 1000;
        }

        if (keycode == 0x25) {
          //k
            countdown = true;
            test_time -= 1000;
        }

        if (keycode & 0x80) {
            if ((keycode & 0x7F) == 0x1D) {
                ctrl = false;
            }

            if ((keycode & 0x7F) == 0x48) {
                countup = false;
            }

            if ((keycode & 0x7F) == 0x50) {
                countdown = false;
            }
        }

        if (keycode == 0x1F) {
            if (ctrl) {
                mode = 0;
                terminal_clear();
                terminal_writestring("Regular Mode!\n");
            }
        }
        if (keycode == 0x1C) {
                mode = 5;
                terminal_clear();
        }

        lastkeycode = keycode;
    //}
}
int ball_x = VGA_WIDTH/2;
int ball_y = VGA_HEIGHT/2;
int velocity_x = 1;
int velocity_y = 1;\
int paddle_x = 0;
int paddle_y = VGA_HEIGHT/2;
int paddle_2_x = VGA_WIDTH-1;
int paddle_2_y = VGA_HEIGHT/2;
char ball = 'O';
bool paddle_1_up = false;
bool paddle_1_down = false;
bool paddle_2_up = false;
bool paddle_2_down = false;
int score[] = {0,0};
char lastkeycode;

void input_wait(float seconds, bool *paddle_1_up, bool *paddle_1_down, bool *paddle_2_up, bool *paddle_2_down) {
    for (volatile int i = 0; i < seconds * (test_time/2); i++) {
        char keycode = read_kbd();
        if (keycode == lastkeycode) {
            continue;
        }
        if (keycode == 0x1D) {
            ctrl = true;
        }
        if(keycode == 0x19){
            if(ctrl){
                // TODO: FIX BEING STUCK IN PONG FOREVER (MODE)
                mode = 0;
                enable_cursor();
                terminal_clear(); 
                terminal_writestring("Regular Mode!\n");
            }
        }
        if (keycode == 0x48) {
            *paddle_1_up = true;
        } else if (keycode == 0x50) {
            *paddle_1_down = true;
        }
        if (keycode & 0x80) {
            if ((keycode & 0x7F) == 0x48) {
                *paddle_1_up = false;
            } else if ((keycode & 0x7F) == 0x50) {
                *paddle_1_down = false;
            } else if ((keycode & 0x7F) == 0x1D) {
                ctrl = false;
            }
        }

        // if (keycode == 0x1E) {
        //     *paddle_2_up = true;
        // } else if (keycode == 0x20) {
        //     *paddle_2_down = true;
        // }
        // if (keycode & 0x80) {
        //     if ((keycode & 0x7F) == 0x1E) {
        //         *paddle_2_up = false;
        //     } else if ((keycode & 0x7F) == 0x20) {
        //         *paddle_2_down = false;
        //     }
        // }

        if (*paddle_1_up) {
            paddle_y--;
        } else if (*paddle_1_down) {
            paddle_y++;
        }

        // if (*paddle_2_up) {
        //     paddle_2_y--;
        // } else if (*paddle_2_down) {
        //     paddle_2_y++;
        // }
        lastkeycode = keycode;
    }
}

void pong_score(int score, int x) {
    switch (score) {
        case 0:
            terminal_writestring_at(" 0000", x, 0);
            terminal_writestring_at("00  00", x, 1);
            terminal_writestring_at("00  00", x, 2);
            terminal_writestring_at("00  00", x, 3);
            terminal_writestring_at(" 0000", x, 4);
            break;
        case 1:
            terminal_writestring_at("1111", x, 0);
            terminal_writestring_at("  11", x, 1);
            terminal_writestring_at("  11", x, 2);
            terminal_writestring_at("  11", x, 3);
            terminal_writestring_at("111111", x, 4);
            break;
        case 2:
            terminal_writestring_at(" 2222", x, 0);
            terminal_writestring_at("22  22", x, 1);
            terminal_writestring_at("   22", x, 2);
            terminal_writestring_at("  22", x, 3);
            terminal_writestring_at("222222", x, 4);
            break;
        case 3:
            terminal_writestring_at(" 3333", x, 0);
            terminal_writestring_at("33  33", x, 1);
            terminal_writestring_at("   333", x, 2);
            terminal_writestring_at("33  33", x, 3);
            terminal_writestring_at(" 3333", x, 4);
            break;
        case 4:
            terminal_writestring_at("44  44", x, 0);
            terminal_writestring_at("44  44", x, 1);
            terminal_writestring_at("444444", x, 2);
            terminal_writestring_at("    44", x, 3);
            terminal_writestring_at("    44", x, 4);
            break;
        case 5:
            terminal_writestring_at("555555", x, 0);
            terminal_writestring_at("55", x, 1);
            terminal_writestring_at("55555", x, 2);
            terminal_writestring_at("    55", x, 3);
            terminal_writestring_at("55555", x, 4);
            break;
        case 6:
            terminal_writestring_at(" 6666", x, 0);
            terminal_writestring_at("66", x, 1);
            terminal_writestring_at("66666", x, 2);
            terminal_writestring_at("66  66", x, 3);
            terminal_writestring_at(" 6666", x, 4);
            break;
        case 7:
            terminal_writestring_at("777777", x, 0);
            terminal_writestring_at("   77", x, 1);
            terminal_writestring_at("  77", x, 2);
            terminal_writestring_at(" 77", x, 3);
            terminal_writestring_at("77", x, 4);
            break;
        case 8:
            terminal_writestring_at(" 8888", x, 0);
            terminal_writestring_at("88  88", x, 1);
            terminal_writestring_at(" 8888", x, 2);
            terminal_writestring_at("88  88", x, 3);
            terminal_writestring_at(" 8888", x, 4);
            break;
        case 9:
            terminal_writestring_at(" 9999", x, 0);
            terminal_writestring_at("99  99", x, 1);
            terminal_writestring_at(" 99999", x, 2);
            terminal_writestring_at("    99", x, 3);
            terminal_writestring_at(" 9999", x, 4);
            break;
        default:
            terminal_column = x;
            terminal_row = 0;
            terminal_writeint(score, 10);
            break;
    }
}

void pong(){
    //test timer?
          if(ball_y >= VGA_HEIGHT-1 || ball_y <= 0){
            velocity_y = -velocity_y;
          }

          if(ball_x >= VGA_WIDTH-1 || ball_x <= 0){
            if(ball_x <= 0){
              if(ball_y >= paddle_y && ball_y <= paddle_y+6){
                velocity_x = -velocity_x;
              } else {
                score[1]++;
                ball_x = VGA_WIDTH/2;
                ball_y = VGA_HEIGHT/2;
              }
            } else if (ball_x >= VGA_WIDTH-1){
              if(ball_y >= paddle_2_y && ball_y <= paddle_2_y+6){
                velocity_x = -velocity_x;
              } else {
                score[0]++;
                ball_x = VGA_WIDTH/2;
                ball_y = VGA_HEIGHT/2;
              }
            }

          }
          ball_x += velocity_x;
          ball_y += velocity_y;

          // ai for now :(
          if(ball_y > paddle_2_y+3){
            paddle_2_y++;
          } else if(ball_y < paddle_2_y+3){
            paddle_2_y--;
          }
          // makes beatable vvv
          paddle_2_y += rand() % 3 - 1;
          //force back in bounds
          if(paddle_2_y < 0){
            paddle_2_y = 0;
          } else if(paddle_2_y > VGA_HEIGHT-6){
            paddle_2_y = VGA_HEIGHT-6;
          }
          if(paddle_y < 0){
            paddle_y = 0;
          } else if(paddle_y > VGA_HEIGHT-6){
            paddle_y = VGA_HEIGHT-6;
          }


          terminal_clear();

          terminal_writestring_at("|\f|\f|\f|\f|\f|", paddle_2_x, paddle_2_y); 
          terminal_writestring_at("|\n|\n|\n|\n|\n|", paddle_x, paddle_y);

          terminal_column = VGA_WIDTH/2-7;
          terminal_row = 0;
          pong_score(score[0], terminal_column);
          terminal_writestring("  :  ");
          pong_score(score[1], terminal_column);

          //balls
          terminal_putentryat(ball, terminal_color, ball_x, ball_y);

          //input and define a clear(-ish) framerate thingy
          input_wait(0.05, &paddle_1_up, &paddle_1_down, &paddle_2_up, &paddle_2_down);
}
bool blink = true;
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
          pong();
        }  else if(mode == 4){
          //settings :o omg change test_time moment
          
          terminal_writestring_at("Interval: ", VGA_WIDTH/2-1, VGA_HEIGHT/2);
          terminal_writeint(test_time, 10);
          terminal_column -= 6;
          terminal_writestring("\n(aim to make the blinky thing below blink every second)\n");
          settings_input_wait(1);
        } else if (mode==5){
          
          if(blink){
            terminal_writestring_at("O",VGA_WIDTH/2-1, VGA_HEIGHT/2);
          } else{
            terminal_writestring_at(".",VGA_WIDTH/2-1, VGA_HEIGHT/2);
          }
          blink = !blink;
          char keycode = read_kbd();
          if ((keycode == 0x01)) {
              terminal_clear();
              mode = 4;
          }
          wait(1);
        } else{
          terminal_writestring("WTF DID YOU DO");
        }
    }
}