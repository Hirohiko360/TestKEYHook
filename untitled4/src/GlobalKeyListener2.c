#include <jni.h>
#include <stdio.h>
#include "GlobalKeyListener.h"
#include <windows.h>
#include <stdlib.h>
#include <time.h>

JNIEXPORT void JNICALL Java_GlobalKeyListener_startListening(JNIEnv *env, jobject obj) {
    printf("F1=Enable/Disable Function\n");
    printf("Left click to start using\n");
    printf("Developed by yby360\n");

    int isRecoilControlActive = 1; // 1 表示激活，0 表示未激活

    // 初始化随机数生成器
    srand(time(NULL));

    // 实现全局键盘监听
    while (1) {
        // 检测 F1 键是否被按下
        if (GetAsyncKeyState(VK_F1) & 0x8000) {
            isRecoilControlActive = !isRecoilControlActive; // 切换状态
            Sleep(300); // 防止重复触发
        }

        if (isRecoilControlActive && (GetAsyncKeyState(VK_LBUTTON) & 0x8000)) {
            printf("VK_LBUTTON_RUN");
            // 模拟鼠标Y轴随机偏移
            for (int i = 0; i < 15; i++) { // 调整循环次数和移动量以适应不同的游戏和武器
                int randomX = (rand() % 5) - 2; // 生成 -2 到 2 之间的随机数
                mouse_event(MOUSEEVENTF_MOVE, randomX, 3, 0, 0);
                Sleep(15); // 调整移动间隔以适应不同的射速
            }
            Sleep(15); // 防止重复触发
        }
        Sleep(5);
    }
}