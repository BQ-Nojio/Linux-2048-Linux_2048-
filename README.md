# Linux-2048-Linux_2048-
Linux系统开发的2048小游戏(Game 2048 Linux System)

在虚拟机系统中要rx xxx.bmp,这些相关资料都在bmp那个文件中，下载过程需要耐心等待

特别注意bgm.wav那个音频文件，它的等待时间会更长

之后记得在“VMware Workstation Pro”中，make clean所有的核心代码，然后再make代码

代码无误之后就可以在虚拟机中rm2048,然后再rx2048

接着就是运行操作，先chmod +x 2048，再./2048，就可以看到一个成功运行的2048游戏

代码都是我成功运行过的，是没有任何问题的

另外我还需要声明一下，这次开发过程非常艰难，我借助了AI来替我完善一些2048额外功能(如重开，悔棋等)

因为还是有蛮多东西没有说明的，比如开发用到的所有软件安装包我没有全部给出

最后，如有疑问，请联系“2381884481@qq.com”这个邮箱，我看到了会回复的，

How to Build and Run

1.In the virtual machine (Ubuntu), download all the required resource files using the rx command (e.g. rx xxx.bmp). 

All image resources are located in the bmp folder.

Please be patient during the download process, especially for the background music file bgm.wav, 

which takes significantly longer to transfer.

2.After downloading the files, navigate to the project directory in VMware Workstation Pro and execute 

the following commands:

Bash

make clean

make

3.Once compilation is successful, transfer the executable to the development board:

Bash

rm 2048          # remove the old version if exists

rx 2048

4.On the ARM development board, run the game with:

Bash

chmod +x 2048

./2048


You should now see the 2048 game running successfully on the 800×480 LCD touchscreen.

Notes


.All code in this repository has been tested and runs successfully on 
 the target hardware with no known issues.
 
.Developing this project was quite challenging. I used AI assistance to help implement 
 and refine several additional features (such as Restart, Undo, background music, etc.).
 
.Due to the complexity of the project, not all development tools and installation packages
 are listed in the repository. Some environment setup steps may need to be adjusted according to your own setup.
 

Contact

If you have any questions or encounter issues, please feel free to contact me at: 2381884481@qq.com. I will reply as soon as I see your message.

