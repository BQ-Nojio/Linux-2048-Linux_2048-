#include "audio.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <errno.h>
#include <sys/soundcard.h>   // OSS 音频 ioctl

#define BGM_FILE_PATH "/home/linux_test/bgm.wav"
#define AUDIO_DEVICE   "/dev/dsp"

/* WAV 文件块标识 */
#define RIFF_TAG 0x46464952
#define WAVE_TAG  0x45564157
#define FMT_TAG   0x20746D66
#define DATA_TAG  0x61746164

/* 音频参数 (与文件匹配) */
#define EXPECTED_SAMPLE_RATE 22050
#define EXPECTED_CHANNELS    1
#define EXPECTED_BITS        16

/* 静态全局变量，供音频线程使用 */
static pthread_t bgm_thread = 0;
static volatile int stop_flag = 0;      // 通知线程退出
static int audio_fd = -1;               // /dev/dsp 文件描述符
static int wav_fd = -1;                 // bgm.wav 文件描述符
static off_t data_start = 0;            // PCM 数据起始偏移
static size_t data_size = 0;            // PCM 数据总大小(字节)

/* 前向声明 */
static void* play_bgm_loop(void *arg);
static int parse_wav_info(const char *path, off_t *data_off, size_t *data_sz);

/* 解析 WAV 文件头，获取 PCM 数据的偏移和长度 */
static int parse_wav_info(const char *path, off_t *data_off, size_t *data_sz)
{
    int fd = open(path, O_RDONLY);
    if (fd == -1) {
        perror("open bgm.wav");
        return -1;
    }

    unsigned int buf[4];
    int ret = -1;

    /* 读取 RIFF 头 */
    if (read(fd, buf, 12) != 12) goto done;
    if (buf[0] != RIFF_TAG || buf[2] != WAVE_TAG) {
        fprintf(stderr, "Not a valid WAV file\n");
        goto done;
    }

    /* 遍历 chunks，找到 fmt 和 data */
    off_t cur = 12;   // 跳过 RIFF 头部12字节
    int found_fmt = 0, found_data = 0;
    while (1) {
        unsigned int chunk_id, chunk_sz;
        if (lseek(fd, cur, SEEK_SET) == -1) break;
        if (read(fd, &chunk_id, 4) != 4) break;
        if (read(fd, &chunk_sz, 4) != 4) break;

        if (chunk_id == FMT_TAG) {
            /* 检查音频格式是否为 PCM (1) */
            unsigned short fmt_tag, channels, bits_per_sample;
            unsigned int sample_rate;
            if (read(fd, &fmt_tag, 2) != 2) break;
            if (fmt_tag != 1) {
                fprintf(stderr, "Only PCM format supported\n");
                goto done;
            }
            if (read(fd, &channels, 2) != 2) break;
            if (read(fd, &sample_rate, 4) != 4) break;
            /* 跳过 byte_rate + block_align */
            if (lseek(fd, 6, SEEK_CUR) == -1) break;
            if (read(fd, &bits_per_sample, 2) != 2) break;

            /* 验证是否与预期一致（可选）*/
            if (sample_rate != EXPECTED_SAMPLE_RATE ||
                channels != EXPECTED_CHANNELS ||
                bits_per_sample != EXPECTED_BITS) {
                fprintf(stderr, "Audio format mismatch: %dHz/%dch/%dbit vs %d/%d/%d\n",
                        sample_rate, channels, bits_per_sample,
                        EXPECTED_SAMPLE_RATE, EXPECTED_CHANNELS, EXPECTED_BITS);
                goto done;
            }
            found_fmt = 1;
        }
        else if (chunk_id == DATA_TAG) {
            *data_off = cur + 8;   // data chunk 数据起始位置
            *data_sz = chunk_sz;
            found_data = 1;
            break;
        }

        /* 跳到下一个 chunk */
        cur += 8 + chunk_sz;
    }

    if (found_fmt && found_data) ret = 0;

done:
    close(fd);
    return ret;
}

/* 音频播放线程函数：循环写入 PCM 数据到 /dev/dsp */
static void* play_bgm_loop(void *arg)
{
    (void)arg;
    char buffer[4096];
    ssize_t bytes_read, bytes_written;
    off_t current_pos;

    /* 打开音频设备 OSS */
    audio_fd = open(AUDIO_DEVICE, O_WRONLY);
    if (audio_fd == -1) {
        perror("open /dev/dsp");
        return NULL;
    }

    /* 设置音频参数 */
    int channels = EXPECTED_CHANNELS;
    if (ioctl(audio_fd, SNDCTL_DSP_CHANNELS, &channels) == -1) {
        perror("ioctl channels");
        goto err;
    }
    if (channels != EXPECTED_CHANNELS) {
        fprintf(stderr, "Failed to set mono channel\n");
        goto err;
    }

    int fmt = AFMT_S16_LE;   // 16位有符号小端
    if (ioctl(audio_fd, SNDCTL_DSP_SETFMT, &fmt) == -1) {
        perror("ioctl setfmt");
        goto err;
    }
    if (fmt != AFMT_S16_LE) {
        fprintf(stderr, "Failed to set 16bit PCM\n");
        goto err;
    }

    int speed = EXPECTED_SAMPLE_RATE;
    if (ioctl(audio_fd, SNDCTL_DSP_SPEED, &speed) == -1) {
        perror("ioctl speed");
        goto err;
    }
    if (speed != EXPECTED_SAMPLE_RATE) {
        fprintf(stderr, "Sample rate mismatch: %d vs %d\n", speed, EXPECTED_SAMPLE_RATE);
        goto err;
    }

    /* 重新打开 WAV 文件用于读取数据（因为线程独立使用）*/
    wav_fd = open(BGM_FILE_PATH, O_RDONLY);
    if (wav_fd == -1) {
        perror("open bgm.wav for reading data");
        goto err;
    }

    /* 循环播放 */
    while (!stop_flag) {
        /* 定位到数据起始处 */
        if (lseek(wav_fd, data_start, SEEK_SET) != data_start) {
            perror("lseek data_start");
            break;
        }
        current_pos = data_start;
        while (current_pos < data_start + data_size && !stop_flag) {
            size_t remain = data_start + data_size - current_pos;
            size_t to_read = (remain < sizeof(buffer)) ? remain : sizeof(buffer);
            bytes_read = read(wav_fd, buffer, to_read);
            if (bytes_read <= 0) {
                break;   // 文件错误或结束
            }
            /* 写入音频设备（可能阻塞）*/
            bytes_written = write(audio_fd, buffer, bytes_read);
            if (bytes_written == -1) {
                perror("write audio device");
                break;
            }
            current_pos += bytes_read;
        }
    }

    /* 清理资源，由 cleanup 统一关闭，此处不重复关闭避免竞态 */
    return NULL;

err:
    if (audio_fd != -1) close(audio_fd);
    if (wav_fd != -1) close(wav_fd);
    audio_fd = -1;
    wav_fd = -1;
    return NULL;
}

/* 对外接口：初始化音频播放 */
int audio_init(void)
{
    off_t data_off = 0;
    size_t data_sz = 0;

    /* 解析 WAV 文件获取数据位置 */
    if (parse_wav_info(BGM_FILE_PATH, &data_off, &data_sz) != 0) {
        fprintf(stderr, "Failed to parse WAV file: %s\n", BGM_FILE_PATH);
        return -1;
    }

    data_start = data_off;
    data_size = data_sz;
    stop_flag = 0;

    /* 创建播放线程 */
    if (pthread_create(&bgm_thread, NULL, play_bgm_loop, NULL) != 0) {
        perror("pthread_create");
        return -1;
    }

    return 0;
}

/* 对外接口：停止播放并释放资源 */
void audio_cleanup(void)
{
    if (bgm_thread != 0) {
        stop_flag = 1;           // 通知线程退出

        /* 可选：向音频设备写入空数据以快速唤醒阻塞的 write */
        if (audio_fd != -1) {
            /* 唤醒写操作，非必须但有助于快速退出 */
            write(audio_fd, NULL, 0);
        }

        pthread_join(bgm_thread, NULL);
        bgm_thread = 0;
    }

    /* 关闭音频设备和文件 */
    if (audio_fd != -1) {
        close(audio_fd);
        audio_fd = -1;
    }
    if (wav_fd != -1) {
        close(wav_fd);
        wav_fd = -1;
    }
    stop_flag = 0;
}