#include "vga.h"
#include "asm_cmds.h"
#include "treedude.h"
#include "time.h"
char lastkeycode;
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
//super hacky way to not use interrupts, read every second
void read_ps2_key(int mode) 
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