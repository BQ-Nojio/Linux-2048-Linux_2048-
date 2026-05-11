
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/mman.h>
#include <math.h>
#include "lcd.h"
#include <stdlib.h>
#include "2048.h"

static int fb_fd = -1; //显示屏文件 的文件描述符。
static int* plcd = NULL; //指向linux帧缓冲区的首地址


void lcd_draw_line(int x0, int y0, int x1, int y1, int color)
{
    int dx = x1 - x0;
    int dy = y1 - y0;

    int abs_dx = dx > 0 ? dx : -dx;
    int abs_dy = dy > 0 ? dy : -dy;

    int sx = (dx > 0) ? 1 : -1;
    int sy = (dy > 0) ? 1 : -1;

    int err = abs_dx - abs_dy;

    while (1)
    {
        lcd_draw_point(x0, y0, color);

        if (x0 == x1 && y0 == y1)
            break;

        int e2 = 2 * err;

        if (e2 > -abs_dy)
        {
            err -= abs_dy;
            x0 += sx;
        }

        if (e2 < abs_dx)
        {
            err += abs_dx;
            y0 += sy;
        }
    }
}




/*
    lcd_open: 打开并映射 显示屏
    无参数、无返回值。
*/
void lcd_open(void)
{
    int fd;
    // 1. 打开 显示屏 文件
    fd = open("/dev/fb0", O_RDWR);
    if (fd == -1)
    {
        printf("failed to open /dev/fb0!!!\n");
        return ;
    }
    fb_fd = fd;
    printf("open /dev/fb0 successfully!\n");

    // 映射 帧缓冲区内存
    plcd = mmap(
        NULL,                   // 映射到哪个地址上去
        800 * 480 * 4,          // 映射区域的长度
        PROT_READ | PROT_WRITE, // 映射区域的权限
        MAP_SHARED,             // 映射标记，“共享映射”
        fd,                     // 文件描述符，表示要映射哪个文件
        0                       // 偏移量，表示从文件的哪个位置开始映射
    );
    if (plcd == MAP_FAILED)
    {
        printf("failed to mmap!\n");
        return ;
    }
}



/*
    lcd_close: 关闭并解映射 显示屏
    无参数、无返回值
*/
void lcd_close(void)
{
    // 3. 关闭 文件
    munmap(plcd, 800 * 480 * 4); // 解映射
    close(fb_fd);
}


/*
    lcd_draw_point: 在屏幕指定位置(x,y)画一个点。
    @x : 像素点的x轴坐标
    @y : 像素点的y轴坐标
    @color: 要显示的颜色值
    无返回值
*/
void lcd_draw_point(int x, int y, int color)
{
    if (x >= 0 && x < 800 && y >= 0 && y < 480)
    {
        *(plcd + 800 * y + x) = color;
    }

}

/*
    lcd_draw_rectangle: 在屏幕指定位置上，画一个指定大小的矩形。
    @x0: 矩形左上顶点的x轴坐标
    @y0: 矩形左上顶点的y轴坐标
    @w:  矩形的宽度
    @h:  矩形的高度
    @fill_color:矩形内部的填充颜色
*/
void lcd_draw_rectangle(int x0, int y0,
                        int w, int h,
                        int fill_color) 
{
    // 参数检查：宽度和高度必须大于0
    if (w <= 0 || h <= 0)
    {
        // 打印错误信息（可选，可根据项目需求去掉）
        printf("lcd_draw_rectangle: 无效的宽度或高度 w=%d, h=%d\n", w, h);
        return;
    }

    // 循环遍历矩形区域的每一个像素点
    for (int y = y0; y < y0 + h; y++)
    {
        for (int x = x0; x < x0 + w; x++)
        {
            // 调用 lcd_draw_point 画点
            lcd_draw_point(x, y, fill_color);
        }
    }
}

void lcd_clear_screen(int color)
{
    lcd_draw_rectangle(0,0,800,480,color);
}

void lcd_draw_star(int color)
{
    int cx = 400;  // 屏幕中心
    int cy = 240;

    int r = 220;   // 半径（留边距）

    int x[5];
    int y[5];

    // 计算五个顶点
    for (int i = 0; i < 5; i++)
    {
        double angle = (-90 + i * 72) * M_PI / 180.0;
        x[i] = cx + r * cos(angle);
        y[i] = cy + r * sin(angle);
    }

    // 按五角星顺序连接
    lcd_draw_line(x[0], y[0], x[2], y[2], color);
    lcd_draw_line(x[2], y[2], x[4], y[4], color);
    lcd_draw_line(x[4], y[4], x[1], y[1], color);
    lcd_draw_line(x[1], y[1], x[3], y[3], color);
    lcd_draw_line(x[3], y[3], x[0], y[0], color);
}