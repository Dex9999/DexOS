bool countup = false;
bool countdown = false;

void settings_input_wait() {
    //for (volatile int elapsed_time = 0; elapsed_time < test_time*seconds; elapsed_time++) {
        if (countup) {
            test_time += 100;
        }
        if (countdown) {
            if(test_time > 100){
                test_time -= 100;
            }
        }

        char keycode = read_kbd();

        if (keycode == lastkeycode) {
            return;
        }

      if (((keycode & 0x7F) == 0x1D && (keycode & 0x80)) || ((keycode & 0x7F) == 0x5B && (keycode & 0x80))) {
           ctrl = false;
       }
       if(keycode == 0x5B || keycode == 0x1D){
         ctrl = true;
       }

        if (keycode == 0x48) {
            countup = true;
            test_time += 100;
        }

        if (keycode == 0x50) {
            countdown = true;
            if(test_time > 100){
            test_time -= 100;
            }
        }

        if (keycode == 0x17) {
          //i
            countup = true;
            test_time += 100;
        }

        if (keycode == 0x25) {
          //k
            countdown = true;
            if(test_time > 100){
            test_time -= 1000;
            }
        }

        if (keycode & 0x80) {
            if ((keycode & 0x7F) == 0x48) {
                countup = false;
            }

            if ((keycode & 0x7F) == 0x50) {
                countdown = false;
            }
        }

        if (keycode == 0x1F) {
            if (ctrl) {
                enable_cursor();
                mode = 0;
                terminal_clear();
                terminal_writestring("Regular Mode!\n");
            } else{
              terminal_clear();
              wait(0.05);
              VGA_HEIGHT += 1;
            }
        }
        // allow user to modify screen size
        if (keycode == 0x11) {
          terminal_clear();
          wait(0.05);
          VGA_HEIGHT -= 1;
        }
        //if (keycode == 0x1f) {
            // in above ctrl thing
        //}
        if (keycode == 0x1e) {
            terminal_clear();
            wait(0.05);
            pong_width_offset_bc_reg_width_breaks_everything += 1;
        }
        if (keycode == 0x20) {
            terminal_clear();
            wait(0.05);
            pong_width_offset_bc_reg_width_breaks_everything -= 1;
        }

        if (keycode == 0x1C) {
                mode = 5;
                terminal_clear();
        }

        lastkeycode = keycode;
    //}
}

void settings(){
  //settings :o omg change test_time moment
  terminal_writestring_at("Interval: ", 2, 1);
  terminal_writeint(test_time, 10);
  terminal_writestring("         \n  (Lower interval means faster ball)");
  terminal_writestring("\n\n  WASD to edit screen size\n  Press Enter to test speed with a DVD screensaver, press Esc to leave it");

  // visual borders
  terminal_writestring_at("+-\f\b|",0,0);
  terminal_writestring_at("|\f\b-+",VGA_WIDTH-1-pong_width_offset_bc_reg_width_breaks_everything,VGA_HEIGHT-2);

  settings_input_wait();
}

char* dvdArt1 =
" _____   _____   ";

char* dvdArt2 =
"|   \\ \\ / /   \\";

char* dvdArt3 =
"| |) \\ V /| |) |";

char* dvdArt4 =
"|___/ \\_/ |___/ ";

int dvdX = 80 / 2;
int dvdY = 25 / 2;
int velocityX = 1;
int velocityY = 1;

void setRandomColour(){
  //typecast int into terminal colour
  terminal_color = vga_entry_color((enum vga_color)(rand() % 16), VGA_COLOR_DARK_GREY);
}

// I have no idea why this makes the logo disappear only sometimes, I give up it works 80% of the time
void updateLogo() {
    if (dvdY >= (int)VGA_HEIGHT - 4 || dvdY < 0) {
        velocityY = -velocityY;
        setRandomColour();
    }

    if (dvdX + 12 >= (int)VGA_WIDTH - 4 - pong_width_offset_bc_reg_width_breaks_everything || dvdX <= 0) {
        velocityX = -velocityX;
        setRandomColour();
    }

      dvdX += velocityX;
      dvdY += velocityY;
}

void screensaver() {
    updateLogo();
    terminal_clear();
    // terminal_writeint(dvdX, 10);
    // terminal_writeint(dvdY, 10);
    // terminal_writeint(VGA_WIDTH, 10);
    terminal_writestring_at(dvdArt1, dvdX, dvdY);
    terminal_column -= 17;
    terminal_row += 1;
    terminal_writestring(dvdArt2);
    terminal_column -= 15;
    terminal_row += 1;
    terminal_writestring(dvdArt3);
    terminal_column -= 16;
    terminal_row += 1;
    terminal_writestring(dvdArt4);



    char keycode = read_kbd();
    if (keycode == 0x01) {
        terminal_color = vga_entry_color(VGA_COLOR_GREEN, VGA_COLOR_DARK_GREY);
        terminal_clear();
        mode = 4;
    }
    wait(1);
}
