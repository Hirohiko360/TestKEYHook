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
            // 模拟鼠标点击
            mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
            Sleep(50);
            mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
            Sleep(1000); // 防止重复触发
        }
        Sleep(10);
    }
}