//uses reg vga width and height but'll switch after literally a frame
int ball_x = 80/2;
int ball_y = 25/2;
int velocity_x = 1;
int velocity_y = 1;\
int paddle_x = 0;
int paddle_y = 25/2;
//moved above
int paddle_2_y = 25/2;
char ball = 'O';
bool paddle_1_up = false;
bool paddle_1_down = false;
bool paddle_2_up = false;
bool paddle_2_down = false;
int score[] = {0,0};
char lastkeycode;



void input_wait(float seconds, bool *paddle_1_up, bool *paddle_1_down) {
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
          if(ball_y >= (int)VGA_HEIGHT-1 || ball_y <= 0){
            velocity_y = -velocity_y;
          }

          if(ball_x >= (int)VGA_WIDTH-1-pong_width_offset_bc_reg_width_breaks_everything || ball_x <= 0){
            if(ball_x <= 0){
              if(ball_y >= paddle_y && ball_y <= paddle_y+6){
                velocity_x = -velocity_x;
              } else {
                score[1]++;
                ball_x = ((int)VGA_WIDTH-pong_width_offset_bc_reg_width_breaks_everything)/2;
                ball_y = (int)VGA_HEIGHT/2;
              }
            } else if (ball_x >= (int)VGA_WIDTH-1-pong_width_offset_bc_reg_width_breaks_everything){
              if(ball_y >= paddle_2_y && ball_y <= paddle_2_y+6){
                velocity_x = -velocity_x;
              } else {
                score[0]++;
                ball_x = ((int)VGA_WIDTH-pong_width_offset_bc_reg_width_breaks_everything)/2;
                ball_y = (int)VGA_HEIGHT/2;
              }
            }

          }
          ball_x += velocity_x;
          ball_y += velocity_y;

          // ai for now :(
          if(ball_y > paddle_2_y+4){
            paddle_2_y++;
          } else if(ball_y < paddle_2_y+4){
            paddle_2_y--;
          }
          // makes beatable vvv
          paddle_2_y += rand() % 3 - 1;
          //force back in bounds
          if(paddle_2_y < 0){
            paddle_2_y = 0;
          } else if(paddle_2_y > (int)VGA_HEIGHT-6){
            paddle_2_y = (int)VGA_HEIGHT-6;
          }
          if(paddle_y < 0){
            paddle_y = 0;
          } else if(paddle_y > (int)VGA_HEIGHT-6){
            paddle_y = (int)VGA_HEIGHT-6;
          }


          terminal_clear();

          terminal_writestring_at("|\f|\f|\f|\f|\f|", paddle_2_x, paddle_2_y); 
          terminal_writestring_at("|\n|\n|\n|\n|\n|", paddle_x, paddle_y);

          terminal_column = (VGA_WIDTH-pong_width_offset_bc_reg_width_breaks_everything)/2-7;
          terminal_row = 0;
          terminal_color = vga_entry_color(VGA_COLOR_RED, VGA_COLOR_DARK_GREY);
          pong_score(score[0], terminal_column);
          terminal_writestring("  :  ");
          pong_score(score[1], terminal_column);
          terminal_color = vga_entry_color(VGA_COLOR_GREEN, VGA_COLOR_DARK_GREY);

          //balls
          terminal_putentryat(ball, terminal_color, ball_x, ball_y);

          //input and define a clear(-ish) framerate thingy
          input_wait(0.05, &paddle_1_up, &paddle_1_down);
}