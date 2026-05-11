#ifndef __2048_H__ 
#define __2048_H__

#define UNDO_X0   0
#define UNDO_Y0   0
#define UNDO_W    100
#define UNDO_H    100

//每行4个棋子
#define ITEM_NUM 4

//这两行用来控制2048棋盘位置
#define MATRIX_X0  190
#define MATRIX_Y0  30


//控制每一个“数字图片”的尺寸
#define ITEM_SIZE 100

//"数字图片"彼此之间的“空白尺寸”
#define BLANK_SIZE 5

//结束界面尺寸
#define GAMEOVER_IMG_WIDTH   450
#define GAMEOVER_IMG_HEIGHT  450

/* 
    save_state: 保存当前矩阵状态，用于悔棋
*/
void save_state(void);

/*
    undo_move: 悔棋，恢复到上一步状态（不添加新数字）
    返回值：
        1 悔棋成功
        0 没有可悔棋的状态
*/
int undo_move(void);

/*
    draw_undo_button: 绘制撤销按钮（显示在屏幕上）
*/
void draw_undo_button(void);


void fill_number_to_matrix_2048(void);

void lcd_draw_matrix_2048(void);

/* 
    is_game_over: 判断游戏是否结束（无空格且无相邻相等元素）
    返回值：
        1 游戏结束
        0 游戏未结束
*/
int is_game_over(void);

/*
    restart_game: 重置棋盘并重新开始游戏
*/
void restart_game(void);


void move_left(void);
void move_right(void);
void move_up(void);
void move_down(void);

void game_2048(void);

#endif