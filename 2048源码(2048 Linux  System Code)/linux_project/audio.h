#ifndef __AUDIO_H__
#define __AUDIO_H__

/*
 * 初始化音频播放：打开音频设备、解析WAV文件、创建播放线程
 * 返回值: 0 成功, -1 失败（不影响游戏继续）
 */
int audio_init(void);

/*
 * 清理音频资源：停止播放线程、关闭文件和设备
 */
void audio_cleanup(void);

#endif