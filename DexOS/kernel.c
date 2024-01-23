#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

//this is my way of screaming at the system vvv
int pong_width_offset_bc_reg_width_breaks_everything = 0;
int paddle_2_x = 80-1;
int mode = 0;

#include "organized/time.h"
#include "organized/treedude.h"
#include "organized/vga.h"
#include "organized/asm_cmds.h"
#include "organized/random.h"
#include "organized/keyboard.h"
#include "organized/settings.h"
#include "organized/pong.h"

void kernel_main(void) 
{
    terminal_initialize();

    // send enable bytes for PS/2 keyboard
    init_kbd();
  //disable_cursor();

    // make sure the text doesn't blink on some computers
    disable_blinking();

    terminal_writestring("Welcome to DexOS!!!! (ctrl-h for help)\n");

    while (1) {
        switch (mode) {
            case 0:
            case 2:
            case 3:
                read_ps2_key();
                update_cursor(terminal_column, terminal_row);
                break;
            case 1:
                pong();
                break;
            case 4:
                settings();
                break;
            case 5:
                screensaver();
                break;
            default:
                terminal_writestring("WHAT DID YOU DO");
                break;
        }
    }
}