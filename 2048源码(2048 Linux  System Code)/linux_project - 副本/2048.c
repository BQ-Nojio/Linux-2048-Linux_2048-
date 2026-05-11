#include <2048.h>
#include <bmp.h>
#include <lcd.h>
#include <stdlib.h>
#include <stdio.h>

#include <sys/ioctl.h> 
#include <string.h>
#include <ts.h>

int matrix_2048[ITEM_NUM][ITEM_NUM]={
    0,0,0,0,
    0,0,0,0,
    0,0,0,0,
    0,0,0,0,
};

/* 悔棋相关 */
static int matrix_2048_prev[ITEM_NUM][ITEM_NUM];  // 上一步矩阵
static int can_undo = 0;                          // 是否有可悔棋的状态


void print_matrix_2048(void)
{
    int i,j;
    printf("**********************\n");
    for (i=0;i<ITEM_NUM;i++)
    {
        for(j=0;j< ITEM_NUM;j++)
        {
            printf("%d ",matrix_2048[i][j]);
        }
        printf("\n");
    }
    printf("**********************\n");
}



int get_zero_number(void)
{
    int n=0;
    int i,j;

    for(i=0; i<ITEM_NUM; i++)
    {
        for (j=0; j<ITEM_NUM; j++)
        {
            if (matrix_2048[i][j]==0)
            {
                n++; 
            }

        }
    }
    
    return n;
}

//游戏结束
/*
    is_game_over: 判断游戏是否结束
    条件：没有空白格子(get_zero_number == 0)
          且没有任意相邻元素相等(上下左右)
*/
int is_game_over(void)
{
    int i, j;

    // 如果有空格，肯定没结束
    if (get_zero_number() > 0)
    {
        return 0;
    }

    // 检查是否有相邻相等的元素（向右、向下检查即可覆盖全部）
    for (i = 0; i < ITEM_NUM; i++)
    {
        for (j = 0; j < ITEM_NUM; j++)
        {
            int val = matrix_2048[i][j];
            if (val == 0)
                continue;

            // 检查右边
            if (j + 1 < ITEM_NUM && matrix_2048[i][j+1] == val)
                return 0;
            // 检查下边
            if (i + 1 < ITEM_NUM && matrix_2048[i+1][j] == val)
                return 0;
        }
    }

    return 1; // 没有空格且无法合并，游戏结束
}


void fill_number_to_matrix_2048(void)
{
    int zero_num = get_zero_number();

    if (zero_num==0)    
    {
        return;
    }

    int pos =random() % zero_num+1;
    int n=0;
    int i,j;

    for (i=0;i< ITEM_NUM;i++)
    {
        for (j=0;j<ITEM_NUM;j++)
        {
            if (matrix_2048[i][j]==0)
            {
                n++;
                if(n==pos)
                {
                    int a[4]={2,4,4,2};
                    int index =random()%4;

                    matrix_2048[i][j]=a[index];
                    print_matrix_2048();
                    return ;
                }
            }
        }
    }
}




void lcd_draw_matrix_2048(void)
{
    int row, col;

    for (row = 0; row < ITEM_NUM; row++)
    {
        for (col = 0; col < ITEM_NUM; col++)
        {
            int x,y;

            x=MATRIX_X0 + (ITEM_SIZE + BLANK_SIZE)*col;
            y=MATRIX_Y0 + (ITEM_SIZE + BLANK_SIZE)*row;

            char pic_name[32];

            sprintf(pic_name, "%d.bmp", matrix_2048[row][col]);

            bmp_display(pic_name, x, y);

        }
    }
}

void move_left(void)
{
    int a;
    int i,k,n;
    for(i = 0; i < ITEM_NUM; i++)
    {
        a=0;//作为“第一个带比较的元素”
        n=0;//前移元素的位置
        for (k = 0; k<ITEM_NUM; k++)
        {
            //找下一个不为0的元素
            if ( matrix_2048[i][k] != 0)
            {
                if (a == 0)
                {
                    //k指向的元素应该作为第一个带比较的元素
                    a = matrix_2048[i][k];
                    matrix_2048[i][k] = 0;
                }
                else
                {
                    if (a == matrix_2048[i][k])
                    {
                        //合并、并前移，并清0、a和matrix_2048[i][k]
                        matrix_2048[i][n++] = a + matrix_2048[i][k];
                        a=0;
                        matrix_2048[i][k] = 0;
                    }
                    else// a != matrix_2048[i][k]
                    {
                        //a前移，matrix_2048[i][k]赋给a,matrix_2048[i][k]变0
                        matrix_2048[i][n++] = a;
                        a = matrix_2048[i][k];
                        matrix_2048[i][k] = 0;
                    }
                }
            }
        }
        if (a != 0)
        {
            matrix_2048[i][n++] = a;
        }
    }
}


void move_right(void)
{
    int a;
    int i,k,n;

    for(i = 0; i < ITEM_NUM; i++)
    {
        a = 0;                      // 第一个带比较的元素
        n = ITEM_NUM - 1;           // 从最右边开始放

        for (k = ITEM_NUM - 1; k >= 0; k--)
        {
            // 找下一个不为0的元素
            if (matrix_2048[i][k] != 0)
            {
                if (a == 0)
                {
                    // 当前作为第一个比较元素
                    a = matrix_2048[i][k];
                    matrix_2048[i][k] = 0;
                }
                else
                {
                    if (a == matrix_2048[i][k])
                    {
                        // 合并并右移
                        matrix_2048[i][n--] = a + matrix_2048[i][k];
                        a = 0;
                        matrix_2048[i][k] = 0;
                    }
                    else
                    {
                        // 不相等：先放a，再更新a
                        matrix_2048[i][n--] = a;
                        a = matrix_2048[i][k];
                        matrix_2048[i][k] = 0;
                    }
                }
            }
        }

        // 最后还有一个a没处理
        if (a != 0)
        {
            matrix_2048[i][n--] = a;
        }
    }
}

void move_up(void)
{
    int a;
    int i,k,n;

    for(i = 0; i < ITEM_NUM; i++)   // i表示列
    {
        a = 0;
        n = 0;                      // 从最上面开始放

        for (k = 0; k < ITEM_NUM; k++)  // 从上往下扫描
        {
            if (matrix_2048[k][i] != 0)
            {
                if (a == 0)
                {
                    a = matrix_2048[k][i];
                    matrix_2048[k][i] = 0;
                }
                else
                {
                    if (a == matrix_2048[k][i])
                    {
                        matrix_2048[n++][i] = a + matrix_2048[k][i];
                        a = 0;
                        matrix_2048[k][i] = 0;
                    }
                    else
                    {
                        matrix_2048[n++][i] = a;
                        a = matrix_2048[k][i];
                        matrix_2048[k][i] = 0;
                    }
                }
            }
        }

        if (a != 0)
        {
            matrix_2048[n++][i] = a;
        }
    }
}


void move_down(void)
{
    int a;
    int i,k,n;

    for(i = 0; i < ITEM_NUM; i++)   // i表示列
    {
        a = 0;
        n = ITEM_NUM - 1;           // 从最下面开始放

        for (k = ITEM_NUM - 1; k >= 0; k--)  // 从下往上扫描
        {
            if (matrix_2048[k][i] != 0)
            {
                if (a == 0)
                {
                    a = matrix_2048[k][i];
                    matrix_2048[k][i] = 0;
                }
                else
                {
                    if (a == matrix_2048[k][i])
                    {
                        matrix_2048[n--][i] = a + matrix_2048[k][i];
                        a = 0;
                        matrix_2048[k][i] = 0;
                    }
                    else
                    {
                        matrix_2048[n--][i] = a;
                        a = matrix_2048[k][i];
                        matrix_2048[k][i] = 0;
                    }
                }
            }
        }

        if (a != 0)
        {
            matrix_2048[n--][i] = a;
        }
    }
}




int change_matrix(move_direction_t mov)
{
    int i,j;
    int matrix_2048_bak[ITEM_NUM][ITEM_NUM];

    //备份矩阵
    for(i=0; i<ITEM_NUM; i++)
    {
        for(j=0; j<ITEM_NUM; j++)
        {
            matrix_2048_bak[i][j]=matrix_2048[i][j];
        }
    }

    if(mov == MOVE_LEFT)
    {
        move_left();
    }
    else if(mov == MOVE_RIGHT)
    {
        move_right();
    }
    else if(mov == MOVE_UP)
    {
        move_up();
    }
    else if(mov == MOVE_DOWN)
    {
        move_down();
    }

    //对比备份矩阵
    for(i=0; i<ITEM_NUM; i++)
    {
        for(j=0; j<ITEM_NUM; j++)
        {
            matrix_2048_bak[i][j]!=matrix_2048[i][j];
            
            return 1;
            
        }
    }

  

}


//save_state: 保存当前矩阵状态，用于悔棋

void save_state(void)
{
    int i, j;
    for (i = 0; i < ITEM_NUM; i++)
        for (j = 0; j < ITEM_NUM; j++)
            matrix_2048_prev[i][j] = matrix_2048[i][j];
}

/*
    undo_move: 悔棋，恢复到上一步状态（不添加新数字）
    返回值：
        1 悔棋成功
        0 没有可悔棋的状态
*/
int undo_move(void)
{
    int i, j;

    if (!can_undo)
        return 0;

    // 恢复矩阵
    for (i = 0; i < ITEM_NUM; i++)
        for (j = 0; j < ITEM_NUM; j++)
            matrix_2048[i][j] = matrix_2048_prev[i][j];

    can_undo = 0;                 // 悔棋后清除标志
    lcd_draw_matrix_2048();       // 重绘棋盘（无新增数字）
    return 1;
}

/*
    draw_undo_button: 绘制撤销按钮（简单矩形 + 文字）
    若准备好 undo.bmp 图片，可直接用 bmp_display 替换
*/
void draw_undo_button(void)
{
    // 绘制按钮背景（灰色）
    //lcd_draw_rectangle(UNDO_X0, UNDO_Y0, UNDO_W, UNDO_H, 0x888888);
    // 若需要显示"撤销"文字，可用图片代替（项目无字体库，建议用图片）
     bmp_display("undo.bmp", UNDO_X0, UNDO_Y0);
}



/*
    restart_game: 重置棋盘并重新开始游戏
    将全局矩阵清零，填充一个随机数字，重绘棋盘
*/
void restart_game(void)
{
    int i, j;

    // 1. 清空矩阵
    for (i = 0; i < ITEM_NUM; i++)
    {
        for (j = 0; j < ITEM_NUM; j++)
        {
            matrix_2048[i][j] = 0;
        }
    }

    can_undo = 0; // 重开时无历史

    // 2. 清屏（可选，保持背景一致）
    lcd_clear_screen(0x0000FF);

    // 3. 随机生成一个数字
    fill_number_to_matrix_2048();

    // 4. 重新绘制棋盘
    lcd_draw_matrix_2048();

    //5. 重绘按钮
    draw_undo_button();         
}




//time事件发生时间;type:事件类型:

//type == EV_KEY  按键事件
//type == EV_ABS absolute  触摸屏事件
//type == EV_REL relative  相对事件、鼠标相对事件

void game_2048()
{
    lcd_clear_screen(0x00FFDD);

    // 1. 随机生成一个数字 填充到 矩阵matrix_2048中去
    fill_number_to_matrix_2048();

    //2. 把2048“棋盘”画出来
    lcd_draw_matrix_2048();

    //3. 绘制撤销按钮
    draw_undo_button();

    while(1)
    {
        //1.等待手指滑动
        move_direction_t mov = get_finger_move_dir();
        //调试
        printf("mov = %d \n",mov);


        //悔棋
        if (mov == MOVE_UNDO)
        {
            undo_move();                    // 悔棋成功/失败均继续循环
            continue;
        }

        // ----- 处理重开 -----
        if (mov == MOVE_RESTART)
        {
            can_undo = 0;                   // 重开清除悔棋状态
            restart_game();
            draw_undo_button();             // 重绘按钮
            continue;
        }

        // ----- 处理滑动 -----
        save_state();                       // 移动前保存状态
        if (change_matrix(mov))             // 矩阵有变化
        {
            can_undo = 1;                   // 允许悔棋
            fill_number_to_matrix_2048();
            lcd_draw_matrix_2048();
            draw_undo_button();             // 确保按钮不被覆盖

            if (is_game_over())
            {
                int gx = (800 - 450) / 2;
                int gy = (480 - 450) / 2;
                bmp_display("gameover.bmp", gx, gy);

                while (1)
                {
                    move_direction_t mov_end = get_finger_move_dir();
                    if (mov_end == MOVE_RESTART)
                    {
                        can_undo = 0;
                        restart_game();
                        draw_undo_button();
                        break;
                    }
                    // 忽略其他输入（包括 UNDO）
                }
            }
        }
        else
        {
            // 矩阵未变化，不更新悔棋状态（can_undo保持不变）
        }

        //3.判断矩阵变化
    }
}


