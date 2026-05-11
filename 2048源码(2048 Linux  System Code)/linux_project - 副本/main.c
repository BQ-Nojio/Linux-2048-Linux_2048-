#include <stdlib.h>
#include <unistd.h>

#include <stdio.h>  //提供 printf 原型

#include <sys/ioctl.h>     // 添加这一行，解决 ioctl 隐式声明警告

#include "ts.h"
#include "2048.h"
#include <time.h>
#include <string.h>
#include "bmp.h"
#include "lcd.h"
// int* plcd=NULL;
#include "audio.h"


int main()
{

    srandom( time(NULL) );
    
    lcd_open();

    // lcd_clear_screen(0x555555);

    // fill_number_to_matrix_2048();

    // lcd_draw_matrix_2048();
 
   /* 游戏开始后启动背景音乐 */
   if (audio_init() == 0) {
        printf("Background music started.\n");
    } else {
        printf("Failed to start background music, continue anyway.\n");
    }

 
    game_2048();

    audio_cleanup();


    lcd_close();

    return 0;
}

