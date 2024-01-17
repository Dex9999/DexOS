#include "vga.h"
#include "asm_cmds.h"

//about a second, let user toggle later (bro did not add customization support)
int test_time = 15000000;

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

bool ctrl = false;

void input_wait(float seconds, bool *paddle_1_up, bool *paddle_1_down, bool *paddle_2_up, bool *paddle_2_down, int *mode) {
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

void pong(int *mode){
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

          terminal_column = VGA_WIDTH/2-1;
          terminal_row = 0;
          terminal_writeint(score[0],10);
          terminal_writestring(" : ");
          terminal_writeint(score[1],10);
          
          //balls
          terminal_putentryat(ball, terminal_color, ball_x, ball_y);
          
          //input and define a clear(-ish) framerate thingy
          input_wait(0.05, &paddle_1_up, &paddle_1_down, &paddle_2_up, &paddle_2_down, &mode);
}