#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "organized/vga.h"
#include "organized/asm_cmds.h"
#include "organized/treedude.h"
#include "organized/pong.h"
#include "organized/keycode.h"
#include "organized/time.h"

int mode = 0;
bool newKey = true;

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
          read_ps2_key(mode);
          update_cursor(terminal_column, terminal_row);
        } else if(mode == 1){
          disable_cursor();
          pong(&mode);
        } else{
          terminal_writestring("WTF DID YOU DO");
        }
    }
}