#include <jni.h>
#include <stdio.h>
#include "GlobalKeyListener2.h"
#include <windows.h>
#include <stdlib.h>
#include <time.h>
#include <mmsystem.h>

#pragma comment(lib, "winmm.lib") // 链接winmm库

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

    int isRecoilControlActive = 0; // 初始状态
    int yOffset = 3; // 初始Y轴偏移量
    int xOffset = 2; // 初始X轴偏移量
    int isAutoClickActive = 0; // 连点功能初始状态
    int bulletCount = 15; // 初始子弹数量
    int fireRate = 15; // 初始射速
    int keyRecoilControl = VK_F1; // 默认按键
    int keyAutoClick = VK_F2; // 默认按键
    int keySaveSettings = VK_F5; // 默认按键
    int keyIncreaseYOffset = VK_F7; // 默认按键
    int keyReduceYOffset = VK_F8; // 默认按键
    int keyIncreaseXOffset = VK_F9; // 默认按键
    int keyReduceXOffset = VK_F10; // 默认按键
    int autoClickDelayBefore = 100; // 连点功能按下鼠标左键后的延迟
    int autoClickDelayAfter = 30; // 连点功能释放鼠标左键前的延迟

    // 尝试从文件中读取设置
    FILE *file = fopen("settings.txt", "r");
    if (file != NULL) {
        fscanf(file, "%d %d %d %d %d %d %d %d %d %d %d %d %d",
               &yOffset, &xOffset, &bulletCount, &fireRate,
               &keyRecoilControl, &keyAutoClick, &keySaveSettings,
               &keyIncreaseYOffset, &keyReduceYOffset, &keyIncreaseXOffset, &keyReduceXOffset,
               &autoClickDelayBefore, &autoClickDelayAfter);
        fclose(file);
        printf("Loaded settings: YOffset = %d, XOffset = %d, BulletCount = %d, FireRate = %d\n", yOffset, xOffset, bulletCount, fireRate);
        printf("Loaded key settings: RecoilControl = %d, AutoClick = %d, SaveSettings = %d, IncreaseYOffset = %d, ReduceYOffset = %d, IncreaseXOffset = %d, ReduceXOffset = %d\n",
               keyRecoilControl, keyAutoClick, keySaveSettings, keyIncreaseYOffset, keyReduceYOffset, keyIncreaseXOffset, keyReduceXOffset);
        printf("Loaded auto click settings: DelayBefore = %d, DelayAfter = %d\n", autoClickDelayBefore, autoClickDelayAfter);
    } else {
        printf("No saved settings found. Using default values.\n");
    }

    // 初始化随机数生成器
    srand(time(NULL));

    // 实现全局键盘监听
    while (1) {
        // 检测 F1 键是否被按下
        if (GetAsyncKeyState(keyRecoilControl) & 0x8000) {
            isRecoilControlActive = !isRecoilControlActive; // 切换下压功能状态
            printf("VK_LBUTTON_UNRUN %s\n", isRecoilControlActive ? "Enabled" : "Disabled");
            Sleep(300); // 防止重复触发
        }

        // 检测 F2 键是否被按下
        if (GetAsyncKeyState(keyAutoClick) & 0x8000) {
            isAutoClickActive = !isAutoClickActive; // 切换连点功能状态
            printf("VK_F2 %s\n", isAutoClickActive ? "Enabled" : "Disabled");
            Sleep(300); // 防止重复触发
        }

        // 检测 F5 键是否被按下
        if (GetAsyncKeyState(keySaveSettings) & 0x8000) {
            FILE *file = fopen("settings.txt", "w");
            if (file != NULL) {
                fprintf(file, "%d %d %d %d %d %d %d %d %d %d %d %d %d\n",
                        yOffset, xOffset, bulletCount, fireRate,
                        keyRecoilControl, keyAutoClick, keySaveSettings,
                        keyIncreaseYOffset, keyReduceYOffset, keyIncreaseXOffset, keyReduceXOffset,
                        autoClickDelayBefore, autoClickDelayAfter);
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
            PlaySound(TEXT(".\\Fun.wav"), NULL, SND_FILENAME | SND_ASYNC); // 播放音乐
            printf("Playing music...\n");
            Sleep(300); // 防止重复触发
        }

        //下压主功能实现
        if (isRecoilControlActive && (GetAsyncKeyState(VK_LBUTTON) & 0x8000)) {
            printf("VK_LBUTTON_RUN\n");

            for (int i = 0; i < bulletCount; i++) { // 调整循环次数和移动量以适应不同的游戏和武器
                int randomX = (rand() % (2 * xOffset + 1)) - xOffset; // 生成 -xOffset 到 xOffset 之间的随机数
                mouse_event(MOUSEEVENTF_MOVE, randomX, yOffset, 0, 0);
                Sleep(fireRate); // 调整移动间隔以适应不同的射速
            }
        }
        Sleep(15); // 防止重复触发

        //连点主功能实现
        if (isAutoClickActive && (GetAsyncKeyState(VK_LBUTTON) & 0x8000)) {
            Sleep(autoClickDelayBefore);
            printf("AutoClickRUN\n");
            mouse_event(MOUSEEVENTF_LEFTDOWN,0,0,0,0);
            Sleep(autoClickDelayAfter);
            mouse_event(MOUSEEVENTF_LEFTUP,0,0,0,0);
        }
        Sleep(2);
    }
}
