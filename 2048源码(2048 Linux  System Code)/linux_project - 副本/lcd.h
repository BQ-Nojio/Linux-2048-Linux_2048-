


#ifndef  __LCD_H__
#define  __LCD_H__

/*
    lcd_open: 打开并映射 显示屏
    无参数、无返回值。
*/
void lcd_open(void);



/*
    lcd_close: 关闭并解映射 显示屏
    无参数、无返回值
*/
void lcd_close(void);


/*
    lcd_draw_point: 在屏幕指定位置(x,y)画一个点。
    @x : 像素点的x轴坐标
    @y : 像素点的y轴坐标
    @color: 要显示的颜色值
    无返回值
*/
void lcd_draw_point(int x, int y, int color);



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
                        int fill_color);

void lcd_draw_line(int x0, int y0, int x1, int y1, int color);
void lcd_draw_star(int color);


void lcd_clear_screen(int color);

#endif 