#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "2048.h"

#include <linux/input.h>
#include <ts.h>


move_direction_t get_finger_move_dir(void)
{
    int fd = open("/dev/input/event0", O_RDONLY);

    if(fd ==-1)
    {
        printf("fucking failed open /dev/input/event0\n");
        return -1;
    }

    struct input_event ev;
    int r;

    //滑动起始点
    int x1=-1,y1=-1;
    //滑动终末点
    int x2,y2;

    while(1)
    {
        r =read(fd, &ev, sizeof(ev));
        if (r != sizeof(ev))
        {
            continue;
        }
        //调试
        printf("type:%x  code:%x  value:%d\n", ev.type, ev.code, ev.value);
    
        //x轴坐标事件
        if(ev.type ==EV_ABS  && ev.code==ABS_X)
        {
            if(x1== -1)
            {
                x1 =ev.value;
      
            }
            x2= ev.value;
        }


         if(ev.type ==EV_ABS  && ev.code==ABS_Y)
        {
            if(y1== -1)
            {
                y1 =ev.value;
              
            }
            y2= ev.value;
        }

        if ((ev.type == EV_KEY && ev.code == BTN_TOUCH && ev.value == 0)
            || (ev.type == EV_ABS && ev.code == ABS_PRESSURE && ev.value == 0))
        {
            int delt_x = abs(x2 - x1);
            int delt_y = abs(y2 - y1);

    // 1. 判断是否为点击
            if (delt_x < TOUCH_CLICK_THRESHOLD && delt_y < TOUCH_CLICK_THRESHOLD)
            {
        // 1.1 判断点击位置是否在撤销按钮区域
                if (x1 >= UNDO_X0 && x1 < UNDO_X0 + UNDO_W &&
                y1 >= UNDO_Y0 && y1 < UNDO_Y0 + UNDO_H)
                {
                    close(fd);
                    return MOVE_UNDO;
                }
                // 1.2 否则视为重开
                close(fd);
                return MOVE_RESTART;
            }
                // 2. 判断水平/垂直滑动
            else if (delt_x >= 2 * delt_y)
            {
                if (x2 > x1)
                {
                    close(fd);
                    return MOVE_RIGHT;
                }
                else
                {
                    close(fd);
                    return MOVE_LEFT;
                }
            }
                else if (delt_y >= 2 * delt_x)
                {
                    if (y2 > y1)
                    {
                        close(fd);
                        return MOVE_DOWN;
                    }
                    else
                    {
                        close(fd);
                        return MOVE_UP;
                    }
                }
                else
                {
                    // 方向不明，重置等待下一次滑动
                    x1 = -1;
                    y1 = -1;
                }
        }
    
    }
}