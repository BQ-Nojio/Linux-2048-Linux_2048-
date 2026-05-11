#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <lcd.h>

void bmp_display(const char* bmp_file, int x0, int y0)
{
    int fd;

    fd = open(bmp_file, O_RDONLY);
    if (fd == -1)
    {
        printf("fucking failed to open bmp file %s\n", bmp_file);
        return ;
    }

    int width, height;

    lseek(fd, 0x12, SEEK_SET);
    read(fd, &width, 4);

    lseek(fd, 0x16, SEEK_SET);
    read(fd, &height, 4);

    short depth;
    lseek(fd, 0x1C, SEEK_SET);
    read(fd, &depth, 2);

    //此处用于调试
    // printf("width = %d\n", width);
    // printf("height = %d\n", height);
    // printf("depth = %d\n",depth);

    if  ( !(depth == 24  || depth == 32) )
    {
        printf("Fuck, Not Support BMP file!\n");
        close(fd);

        return ;
    }
    


    int valid_bytes_per_line;
    int blank=0;
    int total_bytes_per_line;

    valid_bytes_per_line = abs(width) * (depth / 8);

    if(valid_bytes_per_line % 4)
    {
        blank= 4 - valid_bytes_per_line % 4;
    }

    total_bytes_per_line =  valid_bytes_per_line + blank;


    int total_bytes;

    total_bytes = total_bytes_per_line * abs(height);

    unsigned char pixel[total_bytes];
    // unsigned char* pixel = (unsigned char*) malloc(total_bytes);
    lseek(fd, 54, SEEK_SET);
    read(fd, pixel, total_bytes);

    unsigned char a=0,r,g,b; 
    int color;
    int i=0;
    int x,y;

    for(y=0;y<abs(height);y++) 
    {
        for(x=0;x<abs(width);x++)
        {
            b=pixel[i++];
            g=pixel[i++];
            r=pixel[i++];
            if(depth==32)
            {
                a=pixel[i++];
            }
            color=(a<<24)|(r<<16)|(g<<8)|b;

            lcd_draw_point(x0+x,y0+abs(height)-1-y,color);
            
            
        }
        i+=blank;
    }

    // free(pixel);
    close(fd);

    return;
}