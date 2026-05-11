#ifndef __TS_H__

#define __TS_H__

typedef enum move_direction_t
{
    MOVE_UNKNOWN=0,//方向不明
    MOVE_LEFT, 
    MOVE_RIGHT, 
    MOVE_UP, 
    MOVE_DOWN,
    MOVE_RESTART, //点击重开 5
    MOVE_UNDO,  //悔棋 6
}move_direction_t;

move_direction_t get_finger_move_dir(void);

#define TOUCH_CLICK_THRESHOLD 20

#endif