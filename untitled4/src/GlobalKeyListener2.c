#include <jni.h>
#include <stdio.h>
#include "GlobalKeyListener.h"
#include <windows.h>
#include <stdlib.h>
#include <time.h>

JNIEXPORT void JNICALL Java_GlobalKeyListener_startListening(JNIEnv *env, jobject obj) {
    printf("F1 Enable/Disable Function-1\n");
    printf("F2 Enable/Disable Function-2\n");
    printf("F7 Increase Y-axis offset\n");
    printf("F8 Reduce Y-axis offset\n");
    printf("F9 Increase X-axis offset\n");
    printf("F10 Reduce X-axis offset\n");
    printf("Left click to start using\n");
    printf("Developed by yby360\n");
    
    int isRecoilControlActive = 1; // 初始状态
    int yOffset = 3; // 初始Y轴偏移量
    int xOffset = 2; // 初始X轴偏移量
    int isAutoClickActive = 0; // 连点功能初始状态

    // 初始化随机数生成器
    srand(time(NULL));

    // 实现全局键盘监听
    while (1) {
        // 检测 F1 键是否被按下
        if (GetAsyncKeyState(VK_F1) & 0x8000) {
            isRecoilControlActive = !isRecoilControlActive; // 切换下压功能状态
            printf("VK_LBUTTON_UNRUN %s\n", isRecoilControlActive ? "Enabled" : "Disabled");
            Sleep(300); // 防止重复触发
        }

        // 检测 F2 键是否被按下
        if (GetAsyncKeyState(VK_F2) & 0x8000) {
            isAutoClickActive = !isAutoClickActive; // 切换连点功能状态
            printf("VK_F2 %s\n", isAutoClickActive ? "Enabled" : "Disabled");
            Sleep(300); // 防止重复触发
        }

        // 检测F7是否被按下
        if (GetAsyncKeyState(VK_F7) & 0x8000) {
            yOffset += 1; // 增加Y轴偏移量
            printf("YOffset increased to: %d\n", yOffset);
            Sleep(300); // 防止重复触发
        }

        // 检测F8是否被按下
        if (GetAsyncKeyState(VK_F8) & 0x8000) {
            yOffset -= 1; // 减少Y轴偏移量
            printf("YOffset decreased to: %d\n", yOffset);
            Sleep(300); // 防止重复触发
        }

        // 检测F9是否被按下
        if (GetAsyncKeyState(VK_F9) & 0x8000) {
            xOffset += 1; // 增加X轴偏移量
            printf("XOffset increased to: %d\n", xOffset);
            Sleep(300); // 防止重复触发
        }

        // 检测F10是否被按下
        if (GetAsyncKeyState(VK_F10) & 0x8000) {
            xOffset -= 1; // 减少X轴偏移量
            printf("XOffset decreased to: %d\n", xOffset);
            Sleep(300); // 防止重复触发
        }

        //下压主功能实现
        if (isRecoilControlActive && (GetAsyncKeyState(VK_LBUTTON) & 0x8000)) {
            printf("VK_LBUTTON_RUN\n");

            for (int i = 0; i < 15; i++) { // 调整循环次数和移动量以适应不同的游戏和武器
                int randomX = (rand() % (2 * xOffset + 1)) - xOffset; // 生成 -xOffset 到 xOffset 之间的随机数
                mouse_event(MOUSEEVENTF_MOVE, randomX, yOffset, 0, 0);
                Sleep(15); // 调整移动间隔以适应不同的射速
            }
            Sleep(15); // 防止重复触发
        }

        //连点主功能实现
        if (isAutoClickActive && (GetAsyncKeyState(VK_LBUTTON) & 0x8000)) {
                    mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
                    mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
                    Sleep(15); // 调整点击间隔
                }
        Sleep(5);
    }
}