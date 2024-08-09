#include <jni.h>
#include <stdio.h>
#include "GlobalKeyListener.h"
#include <windows.h>

JNIEXPORT void JNICALL Java_GlobalKeyListener_startListening(JNIEnv *env, jobject obj) {
    // 实现全局键盘监听
    // 这里是一个简单的示例，监听 M 键
    while (1) {
        if (GetAsyncKeyState('M') & 0x8000) {
            printf("M 键被按下\n");
            // 模拟鼠标连续下移
            for (int i = 0; i < 10; i++) { // 调整循环次数和移动量以适应不同的游戏和武器
                mouse_event(MOUSEEVENTF_MOVE, 0, 5, 0, 0); // 每次移动5个像素
                Sleep(50); // 调整移动间隔以适应不同的射速
            }
            Sleep(1000); // 防止重复触发
        }
        Sleep(10);
    }
}