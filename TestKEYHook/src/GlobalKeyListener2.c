#include <jni.h>
#include <stdio.h>
#include "GlobalKeyListener2.h"
#include <windows.h>
#include <stdlib.h>
#include <time.h>
#include <mmsystem.h>
#include <stdbool.h>

#pragma comment(lib, "winmm.lib") // 链接winmm库

// 全局变量，用于存储线程状态和设置
volatile int isRecoilControlActive = 0; // 初始状态
volatile int yOffset = 3; // 初始Y轴偏移量
volatile int xOffset = 1; // 初始X轴偏移量
volatile int isAutoClickActive = 0; // 连点功能初始状态
volatile int bulletCount = 15; // 初始子弹数量
volatile int fireRate = 15; // 初始射速
volatile int keyRecoilControl = VK_F1; // 默认按键
volatile int keyAutoClick = VK_F2; // 默认按键
volatile int keySaveSettings = VK_F5; // 默认按键
volatile int keyIncreaseYOffset = VK_F7; // 默认按键
volatile int keyReduceYOffset = VK_F8; // 默认按键
volatile int keyIncreaseXOffset = VK_F9; // 默认按键
volatile int keyReduceXOffset = VK_F10; // 默认按键
volatile int autoClickDelayBefore = 10; // 连点功能按下鼠标左键后的延迟
volatile int autoClickDelayAfter = 10; // 连点功能释放鼠标左键前的延迟
volatile int clickCount = 4; // 自定义点击次数
volatile int isMusicPlaying = 0; // 音乐播放状态
volatile int recoilResetMultiplier = 2; // 初始值为10
volatile int maxHoldTime = 2000; // 最大按住时间
volatile int holdTimeMultiplier = 1; // 按住时间乘数
volatile int singleShotThreshold = 100; // 单发阈值，单位为毫秒
volatile int singleShotYOffset = 2; // 新增：单发下压量

// 声音文件路径
const char* enableSoundPath = ".\\Enable.wav";
const char* disableSoundPath = ".\\Disable.wav";

// 播放声音函数
void PlayEnableSound() {
    PlaySound(enableSoundPath, NULL, SND_FILENAME | SND_ASYNC);
}

void PlayDisableSound() {
    PlaySound(disableSoundPath, NULL, SND_FILENAME | SND_ASYNC);
}

// 键盘监听线程函数
DWORD WINAPI KeyboardListenerThread(LPVOID lpParam) {
    while (1) {
        // 检测 F1 键是否被按下
        if (GetAsyncKeyState(keyRecoilControl) & 0x8000) {
            isRecoilControlActive = !isRecoilControlActive; // 切换下压功能状态
            if (isRecoilControlActive) {
                PlayEnableSound();
            } else {
                PlayDisableSound();
            }
            printf("VK_LBUTTON_UNRUN %s\n", isRecoilControlActive ? "Enabled" : "Disabled");
            Sleep(300); // 防止重复触发
        }

        // 检测 F2 键是否被按下
        if (GetAsyncKeyState(keyAutoClick) & 0x8000) {
            isAutoClickActive = !isAutoClickActive; // 切换连点功能状态
            if (isAutoClickActive) {
                PlayEnableSound();
            } else {
                PlayDisableSound();
            }
            printf("VK_F2 %s\n", isAutoClickActive ? "Enabled" : "Disabled");
            Sleep(300); // 防止重复触发
        }

        // 检测 F5 键是否被按下
        if (GetAsyncKeyState(keySaveSettings) & 0x8000) {
            FILE *file = fopen("settings.txt", "w");
            if (file != NULL) {
                fprintf(file, "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",
                        yOffset, xOffset, bulletCount, fireRate,
                        keyRecoilControl, keyAutoClick, keySaveSettings,
                        keyIncreaseYOffset, keyReduceYOffset, keyIncreaseXOffset, keyReduceXOffset,
                        autoClickDelayBefore, autoClickDelayAfter, clickCount, recoilResetMultiplier,
                        maxHoldTime, holdTimeMultiplier, singleShotThreshold, singleShotYOffset);
                fclose(file);
                printf("Current settings saved.\n");
            } else {
                printf("Failed to save settings.\n");
            }
            Sleep(300); // 防止重复触发
        }

        // 检测F7是否被按下
        if (GetAsyncKeyState(keyIncreaseYOffset) & 0x8000) {
            yOffset += 1; // 增加Y轴偏移量
            printf("YOffset increased to: %d\n", yOffset);
            Sleep(300); // 防止重复触发
        }

        // 检测F8是否被按下
        if (GetAsyncKeyState(keyReduceYOffset) & 0x8000) {
            yOffset -= 1; // 减少Y轴偏移量
            printf("YOffset decreased to: %d\n", yOffset);
            Sleep(300); // 防止重复触发
        }

        // 检测F9是否被按下
        if (GetAsyncKeyState(keyIncreaseXOffset) & 0x8000) {
            xOffset += 1; // 增加X轴偏移量
            printf("XOffset increased to: %d\n", xOffset);
            Sleep(300); // 防止重复触发
        }

        // 检测F10是否被按下
        if (GetAsyncKeyState(keyReduceXOffset) & 0x8000) {
            xOffset -= 1; // 减少X轴偏移量
            printf("XOffset decreased to: %d\n", xOffset);
            Sleep(300); // 防止重复触发
        }

        // 检测F12是否被按下
        if (GetAsyncKeyState(VK_F12) & 0x8000) {
            if (isMusicPlaying) {
                PlaySound(NULL, 0, 0); // 停止播放音乐
                printf("Music paused.\n");
            } else {
                PlaySound(TEXT(".\\Fun.wav"), NULL, SND_FILENAME | SND_ASYNC); // 播放音乐
                printf("Playing music...\n");
            }
            isMusicPlaying = !isMusicPlaying; // 切换音乐播放状态
            Sleep(300); // 防止重复触发
        }

        Sleep(5); // 防止重复触发
    }
    return 0;
}

// 平滑函数
int smoothStep(int edge0, int edge1, int x) {
    if (x < edge0) return 0;
    if (x >= edge1) return 1;
    x = (x - edge0) / (float)(edge1 - edge0);
    return x * x * (3 - 2 * x);
}

// 下压功能线程函数
DWORD WINAPI RecoilControlThread(LPVOID lpParam) {
    bool wasFiring = false; // 新增变量，用于记录是否在开火
    DWORD startTime = 0; // 记录按下鼠标左键的时间
    DWORD currentTime = 0; // 当前时间
    DWORD holdTime = 0; // 按住时间

    while (1) {
        if (isRecoilControlActive && (GetAsyncKeyState(VK_LBUTTON) & 0x8000)) {
            if (!wasFiring) {
                startTime = GetTickCount(); // 记录按下鼠标左键的时间
                wasFiring = true;
            }
            currentTime = GetTickCount();
            holdTime = currentTime - startTime; // 计算按住时间

            printf("VK_LBUTTON_RUN\n");

            if (holdTime > singleShotThreshold) { // 如果按住时间超过单发阈值，认为是连发
                for (int i = 0; i < bulletCount; i++) {
                    int dynamicYOffset = yOffset + smoothStep(0, maxHoldTime, holdTime) * holdTimeMultiplier * yOffset; // 动态调整Y轴偏移量，平滑过渡
                    dynamicYOffset = min(dynamicYOffset, yOffset * 2); // 限制最大偏移量为初始偏移量的2倍

                    int randomX = (rand() % (2 * xOffset + 1)) - xOffset; // 生成 -xOffset 到 xOffset 之间的随机数
                    mouse_event(MOUSEEVENTF_MOVE, randomX, dynamicYOffset, 0, 0);
                    Sleep(fireRate); // 调整移动间隔以适应不同的射速
                }
            } else { // 单发情况
                int halfYOffset = singleShotYOffset; // 使用配置中的单发下压量
                int randomX = (rand() % (2 * xOffset + 1)) - xOffset; // 生成 -xOffset 到 xOffset 之间的随机数
                mouse_event(MOUSEEVENTF_MOVE, randomX, halfYOffset, 0, 0);
            }
        } else if (isRecoilControlActive && !(GetAsyncKeyState(VK_LBUTTON) & 0x8000) && wasFiring) {
            // 当鼠标左键松开时，并且之前在开火，才自动上抬鼠标指针
            printf("VK_LBUTTON_RUN_RESET\n");
            if (holdTime > singleShotThreshold) { // 如果按住时间超过单发阈值，认为是连发
                mouse_event(MOUSEEVENTF_MOVE, 0, -recoilResetMultiplier * yOffset, 0, 0); // 上抬鼠标指针
            } else { // 单发情况
                mouse_event(MOUSEEVENTF_MOVE, 0, -recoilResetMultiplier * singleShotYOffset, 0, 0); // 上抬鼠标指针，使用配置中的单发下压量
            }
            wasFiring = false; // 重置开火状态
            holdTime = 0; // 重置按住时间
        }
        Sleep(5); // 防止重复触发
    }
    return 0;
}

// 连点功能线程函数
DWORD WINAPI AutoClickThread(LPVOID lpParam) {
    while (1) {
        if (isAutoClickActive && (GetAsyncKeyState(VK_LBUTTON) & 0x8000)) {
            // 等待鼠标左键抬起
            while (GetAsyncKeyState(VK_LBUTTON) & 0x8000) {
                Sleep(5); // 短暂休眠，避免CPU占用过高
            }

            int currentClickCount = clickCount; // 使用配置中的点击次数

            while (currentClickCount > 0) {
                Sleep(autoClickDelayBefore);
                printf("AutoClickRUN\n");
                mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
                Sleep(autoClickDelayAfter);
                mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
                currentClickCount--;
            }
        }
        Sleep(5); // 防止重复触发
    }
    return 0;
}

JNIEXPORT void JNICALL Java_GlobalKeyListener2_startListening(JNIEnv *env, jobject obj) {
    printf("F1 Enable/Disable Function-1\n");
    printf("F2 Enable/Disable Function-2\n");
    printf("F5 Save Current Settings\n");
    printf("F7 Increase Y-axis offset\n");
    printf("F8 Reduce Y-axis offset\n");
    printf("F9 Increase X-axis offset\n");
    printf("F10 Reduce X-axis offset\n");
    printf("Left click to start using\n");
    printf("Developed by yby360\n");

    // 尝试从文件中读取设置
    FILE *file = fopen("settings.txt", "r");
    if (file != NULL) {
        fscanf(file, "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d",
               &yOffset, &xOffset, &bulletCount, &fireRate,
               &keyRecoilControl, &keyAutoClick, &keySaveSettings,
               &keyIncreaseYOffset, &keyReduceYOffset, &keyIncreaseXOffset, &keyReduceXOffset,
               &autoClickDelayBefore, &autoClickDelayAfter, &clickCount, &recoilResetMultiplier,
               &maxHoldTime, &holdTimeMultiplier, &singleShotThreshold, &singleShotYOffset);
        fclose(file);
        printf("Loaded settings: YOffset = %d, XOffset = %d, BulletCount = %d, FireRate = %d\n", yOffset, xOffset, bulletCount, fireRate);
        printf("Loaded key settings: RecoilControl = %d, AutoClick = %d, SaveSettings = %d, IncreaseYOffset = %d, ReduceYOffset = %d, IncreaseXOffset = %d, ReduceXOffset = %d\n",
               keyRecoilControl, keyAutoClick, keySaveSettings, keyIncreaseYOffset, keyReduceYOffset, keyIncreaseXOffset, keyReduceXOffset);
        printf("Loaded auto click settings: DelayBefore = %d, DelayAfter = %d, ClickCount = %d\n", autoClickDelayBefore, autoClickDelayAfter, clickCount);
        printf("Loaded recoil reset multiplier: %d\n", recoilResetMultiplier);
        printf("Loaded max hold time: %d\n", maxHoldTime);
        printf("Loaded hold time multiplier: %d\n", holdTimeMultiplier);
        printf("Loaded single shot threshold: %d\n", singleShotThreshold);
        printf("Loaded single shot Y offset: %d\n", singleShotYOffset);
    } else {
        printf("No saved settings found. Using default values.\n");
    }

    // 初始化随机数生成器
    srand(time(NULL));

    // 创建键盘监听线程
    HANDLE keyboardThread = CreateThread(NULL, 0, KeyboardListenerThread, NULL, 0, NULL);
    if (keyboardThread == NULL) {
        printf("Failed to create keyboard listener thread.\n");
        return;
    }

    // 创建下压功能线程
    HANDLE recoilThread = CreateThread(NULL, 0, RecoilControlThread, NULL, 0, NULL);
    if (recoilThread == NULL) {
        printf("Failed to create recoil control thread.\n");
        return;
    }

    // 创建连点功能线程
    HANDLE autoClickThread = CreateThread(NULL, 0, AutoClickThread, NULL, 0, NULL);
    if (autoClickThread == NULL) {
        printf("Failed to create auto click thread.\n");
        return;
    }

    // 等待线程结束（实际上这些线程是无限循环的，不会结束）
    WaitForSingleObject(keyboardThread, INFINITE);
    WaitForSingleObject(recoilThread, INFINITE);
    WaitForSingleObject(autoClickThread, INFINITE);
}
