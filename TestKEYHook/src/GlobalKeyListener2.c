#include <jni.h>
#include <stdio.h>
#include "GlobalKeyListener2.h"
#include <windows.h>
#include <stdlib.h>
#include <time.h>
#include <mmsystem.h>
#include <stdbool.h>

#pragma comment(lib, "winmm.lib") // 链接winmm库

// 定义颜色
#define DEFAULT_COLOR 7 // 默认颜色（白色）
#define GREEN_COLOR 10  // 绿色
#define RED_COLOR 12    // 红色
#define BLUE_COLOR 9    // 蓝色

// 设置控制台文字颜色的函数
void SetConsoleColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

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
volatile int recoilResetMultiplier = 2; // 初始值为2

// 声音文件路径
const char* enableSoundPath = ".\\Enable.wav";
const char* disableSoundPath = ".\\Disable.wav";

// 声明窗口
HWND hwnd;

// 窗口过程函数
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            // 创建一个带有换行符的字符串
            char buffer[256];
            snprintf(buffer, sizeof(buffer),
                     "Recoil Control: %s\n"
                     "YOffset: %d\n"
                     "XOffset: %d\n"
                     "Bullet Count: %d\n"
                     "Fire Rate: %d\n",
                     isRecoilControlActive ? "Enabled" : "Disabled",
                     yOffset,
                     xOffset,
                     bulletCount,
                     fireRate);

            // 使用TextOut逐行输出
            int startY = 10; // 起始Y坐标
            char *line = strtok(buffer, "\n"); // 获取每一行
            while (line != NULL) {
                TextOut(hdc, 10, startY, line, strlen(line)); // 输出当前行
                startY += 20; // 每行间隔20个像素
                line = strtok(NULL, "\n"); // 获取下一行
            }

            EndPaint(hwnd, &ps);
        }
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

// 创建GUI窗口
void CreateGuiWindow() {
    const char CLASS_NAME[] = "ParameterWindowClass";

    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    hwnd = CreateWindowEx(
        WS_EX_TOPMOST, // 使窗口始终在最上面
        CLASS_NAME,
        "Current Parameters",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 300, 200,
        NULL, NULL, GetModuleHandle(NULL), NULL);

    ShowWindow(hwnd, SW_SHOW);
}

// 更新GUI
void UpdateGui() {
    InvalidateRect(hwnd, NULL, TRUE); // 请求重绘窗口
}

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
            SetConsoleColor(isRecoilControlActive ? GREEN_COLOR : RED_COLOR);
            printf("VK_LBUTTON_UNRUN %s\n", isRecoilControlActive ? "Enabled" : "Disabled");
            SetConsoleColor(DEFAULT_COLOR); // 恢复默认颜色
            if (isRecoilControlActive) {
                PlayEnableSound();
                } else {
                PlayDisableSound();
            }
            Sleep(300); // 防止重复触发
        }

        // 检测 F2 键是否被按下
        if (GetAsyncKeyState(keyAutoClick) & 0x8000) {
            isAutoClickActive = !isAutoClickActive; // 切换连点功能状态
            SetConsoleColor(isAutoClickActive ? GREEN_COLOR : RED_COLOR);
            printf("VK_F2 %s\n", isAutoClickActive ? "Enabled" : "Disabled");
            SetConsoleColor(DEFAULT_COLOR); // 恢复默认颜色
            if (isAutoClickActive) {
                PlayEnableSound();
                } else {
                PlayDisableSound();
            }
            Sleep(300); // 防止重复触发
        }

        // 检测 F5 键是否被按下
        if (GetAsyncKeyState(keySaveSettings) & 0x8000) {
            FILE *file = fopen("settings.txt", "w");
            if (file != NULL) {
                fprintf(file, "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",
                        yOffset, xOffset, bulletCount, fireRate,
                        keyRecoilControl, keyAutoClick, keySaveSettings,
                        keyIncreaseYOffset, keyReduceYOffset, keyIncreaseXOffset, keyReduceXOffset,
                        autoClickDelayBefore, autoClickDelayAfter, clickCount, recoilResetMultiplier);
                fclose(file);
                SetConsoleColor(GREEN_COLOR);
                printf("Current settings saved.\n");
            } else {
                SetConsoleColor(RED_COLOR);
                printf("Failed to save settings.\n");
            }
            SetConsoleColor(DEFAULT_COLOR); // 恢复默认颜色
            Sleep(300); // 防止重复触发
        }

        // 检测F7是否被按下
        if (GetAsyncKeyState(keyIncreaseYOffset) & 0x8000) {
            yOffset += 1; // 增加Y轴偏移量
            SetConsoleColor(BLUE_COLOR);
            printf("YOffset increased to: %d\n", yOffset);
            SetConsoleColor(DEFAULT_COLOR); // 恢复默认颜色
            Sleep(300); // 防止重复触发
        }

        // 检测F8是否被按下
        if (GetAsyncKeyState(keyReduceYOffset) & 0x8000) {
            yOffset -= 1; // 减少Y轴偏移量
            SetConsoleColor(BLUE_COLOR);
            printf("YOffset decreased to: %d\n", yOffset);
            SetConsoleColor(DEFAULT_COLOR); // 恢复默认颜色
            Sleep(300); // 防止重复触发
        }

        // 检测F9是否被按下
        if (GetAsyncKeyState(keyIncreaseXOffset) & 0x8000) {
            xOffset += 1; // 增加X轴偏移量
            SetConsoleColor(BLUE_COLOR);
            printf("XOffset increased to: %d\n", xOffset);
            SetConsoleColor(DEFAULT_COLOR); // 恢复默认颜色
            Sleep(300); // 防止重复触发
        }

        // 检测F10是否被按下
        if (GetAsyncKeyState(keyReduceXOffset) & 0x8000) {
            xOffset -= 1; // 减少X轴偏移量
            SetConsoleColor(BLUE_COLOR);
            printf("XOffset decreased to: %d\n", xOffset);
            SetConsoleColor(DEFAULT_COLOR); // 恢复默认颜色
            Sleep(300); // 防止重复触发
        }

        // 检测F12是否被按下
        if (GetAsyncKeyState(VK_F12) & 0x8000) {
            SetConsoleColor(isMusicPlaying ? RED_COLOR : GREEN_COLOR);
            if (isMusicPlaying) {
                PlaySound(NULL, 0, 0); // 停止播放音乐
                printf("Music paused.\n");
            } else {
                PlaySound(TEXT(".\\Fun.wav"), NULL, SND_FILENAME | SND_ASYNC); // 播放音乐
                printf("Playing music...\n");
            }
            SetConsoleColor(DEFAULT_COLOR); // 恢复默认颜色
            isMusicPlaying = !isMusicPlaying; // 切换音乐播放状态
            Sleep(300); // 防止重复触发
        }

        Sleep(5); // 防止重复触发
        UpdateGui(); // 更新GUI
    }
    return 0;
}

// 下压功能线程函数
DWORD WINAPI RecoilControlThread(LPVOID lpParam) {
    bool wasFiring = false; // 新增变量，用于记录是否在开火

    while (1) {
        if (isRecoilControlActive && (GetAsyncKeyState(VK_LBUTTON) & 0x8000)) {
            printf("VK_LBUTTON_RUN\n");

            int currentYOffset = yOffset; // 默认使用全量下压
            if (!(GetAsyncKeyState(VK_RBUTTON) & 0x8000)) {
                // 如果右键没有按下，视为非瞄准模式，下压程度为二分之一
                currentYOffset /= 2;
            }

            for (int i = 0; i < bulletCount; i++) { // 调整循环次数和移动量以适应不同的游戏和武器
                int randomX = (rand() % (2 * xOffset + 1)) - xOffset; // 生成 -xOffset 到 xOffset 之间的随机数
                mouse_event(MOUSEEVENTF_MOVE, randomX, currentYOffset, 0, 0);
                Sleep(fireRate); // 调整移动间隔以适应不同的射速
            }
            wasFiring = true; // 记录当前正在开火
        } else if (isRecoilControlActive && !(GetAsyncKeyState(VK_LBUTTON) & 0x8000) && wasFiring) {
            // 当鼠标左键松开时，并且之前在开火，才自动上抬鼠标指针
            printf("VK_LBUTTON_RUN_RESET\n");
            mouse_event(MOUSEEVENTF_MOVE, 0, -recoilResetMultiplier * yOffset, 0, 0); // 上抬鼠标指针
            wasFiring = false; // 重置开火状态
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
    SetConsoleColor(BLUE_COLOR);
    printf("F1 Enable/Disable Function-1\n");
    printf("F2 Enable/Disable Function-2\n");
    printf("F5 Save Current Settings\n");
    printf("F7 Increase Y-axis offset\n");
    printf("F8 Reduce Y-axis offset\n");
    printf("F9 Increase X-axis offset\n");
    printf("F10 Reduce X-axis offset\n");
    printf("Left click to start using\n");
    printf("Developed by yby360\n");
    SetConsoleColor(DEFAULT_COLOR); // 恢复默认颜色

    SetConsoleColor(BLUE_COLOR);
    printf("Starting parameter GUI...\n");
    CreateGuiWindow(); // 创建GUI窗口

    // 尝试从文件中读取设置
    FILE *file = fopen("settings.txt", "r");
    if (file != NULL) {
        fscanf(file, "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d",
               &yOffset, &xOffset, &bulletCount, &fireRate,
               &keyRecoilControl, &keyAutoClick, &keySaveSettings,
               &keyIncreaseYOffset, &keyReduceYOffset, &keyIncreaseXOffset, &keyReduceXOffset,
               &autoClickDelayBefore, &autoClickDelayAfter, &clickCount, &recoilResetMultiplier);
        fclose(file);
        SetConsoleColor(GREEN_COLOR);
        printf("Loaded settings: YOffset = %d, XOffset = %d, BulletCount = %d, FireRate = %d\n", yOffset, xOffset, bulletCount, fireRate);
        printf("Loaded key settings: RecoilControl = %d, AutoClick = %d, SaveSettings = %d, IncreaseYOffset = %d, ReduceYOffset = %d, IncreaseXOffset = %d, ReduceXOffset = %d\n",
               keyRecoilControl, keyAutoClick, keySaveSettings, keyIncreaseYOffset, keyReduceYOffset, keyIncreaseXOffset, keyReduceXOffset);
        printf("Loaded auto click settings: DelayBefore = %d, DelayAfter = %d, ClickCount = %d\n", autoClickDelayBefore, autoClickDelayAfter, clickCount);
        printf("Loaded recoil reset multiplier: %d\n", recoilResetMultiplier);
        SetConsoleColor(DEFAULT_COLOR); // 恢复默认颜色
    } else {
        SetConsoleColor(RED_COLOR);
        printf("No saved settings found. Using default values.\n");
        SetConsoleColor(DEFAULT_COLOR); // 恢复默认颜色
    }

    // 初始化随机数生成器
    srand(time(NULL));

    // 创建键盘监听线程
    HANDLE keyboardThread = CreateThread(NULL, 0, KeyboardListenerThread, NULL, 0, NULL);
    if (keyboardThread == NULL) {
        SetConsoleColor(RED_COLOR);
        printf("Failed to create keyboard listener thread.\n");
        SetConsoleColor(DEFAULT_COLOR); // 恢复默认颜色
        return;
    }

    // 创建下压功能线程
    HANDLE recoilThread = CreateThread(NULL, 0, RecoilControlThread, NULL, 0, NULL);
    if (recoilThread == NULL) {
        SetConsoleColor(RED_COLOR);
        printf("Failed to create recoil control thread.\n");
        SetConsoleColor(DEFAULT_COLOR); // 恢复默认颜色
        return;
    }

    // 创建连点功能线程
    HANDLE autoClickThread = CreateThread(NULL, 0, AutoClickThread, NULL, 0, NULL);
    if (autoClickThread == NULL) {
        SetConsoleColor(RED_COLOR);
        printf("Failed to create auto click thread.\n");
        SetConsoleColor(DEFAULT_COLOR); // 恢复默认颜色
        return;
    }

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // 等待线程结束（实际上这些线程是无限循环的，不会结束）
    WaitForSingleObject(keyboardThread, INFINITE);
    WaitForSingleObject(recoilThread, INFINITE);
    WaitForSingleObject(autoClickThread, INFINITE);
}
