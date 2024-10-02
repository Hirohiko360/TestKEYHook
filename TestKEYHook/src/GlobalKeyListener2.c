#include <jni.h>
#include <stdio.h>
#include "GlobalKeyListener2.h"
#include <windows.h>
#include <stdlib.h>
#include <time.h>
#include <mmsystem.h>
#include <stdbool.h>
#include <shellapi.h>

#pragma comment(lib, "winmm.lib") // 链接 winmm 库

// 定义颜色
#define DEFAULT_COLOR 7 // 默认颜色（白色）
#define GREEN_COLOR 10  // 绿色
#define RED_COLOR 12    // 红色
#define BLUE_COLOR 9    // 蓝色
#define YELLOW_COLOR 14  // 黄色

// 设置控制台文本颜色的函数
void SetConsoleColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

// 全局变量存储线程状态和设置
volatile int isRecoilControlActive = 0; // 初始状态
volatile int yOffset = 3; // 初始Y轴偏移
volatile int xOffset = 1; // 初始X轴偏移
volatile int isAutoClickActive = 0; // 自动点击功能的初始状态
volatile int bulletCount = 15; // 初始子弹数量
volatile int fireRate = 15; // 初始射速
volatile int keyRecoilControl = VK_F1; // 默认键
volatile int keyAutoClick = VK_F2; // 默认键
volatile int keySaveSettings = VK_F5; // 默认键
volatile int keyIncreaseYOffset = VK_F7; // 默认键
volatile int keyReduceYOffset = VK_F8; // 默认键
volatile int keyIncreaseXOffset = VK_F9; // 默认键
volatile int keyReduceXOffset = VK_F10; // 默认键
volatile int autoClickDelayBefore = 10; // 自动点击开始前的延迟
volatile int autoClickDelayAfter = 10; // 释放鼠标按钮后的延迟
volatile int clickCount = 4; // 自定义点击计数
volatile int isMusicPlaying = 0; // 音乐播放状态
volatile int recoilResetMultiplier = 2; // 初始值为2
volatile int recoilDelay = 500; // 初始值为500毫秒
volatile int triggerMode = 0; // 触发模式，0代表左键触发，1代表同时按下两个按钮

// 音效文件路径
const char* enableSoundPath = ".\\Enable.wav";
const char* disableSoundPath = ".\\Disable.wav";

// 声明窗口
HWND hwnd;

// 托盘图标数据
#define WM_TRAYICON (WM_USER + 1) // 自定义消息
NOTIFYICONDATA nid; // 托盘图标数据

// 窗口过程函数
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_DESTROY:
            Shell_NotifyIcon(NIM_DELETE, &nid); // 删除托盘图标
            PostQuitMessage(0);
            return 0;

        case WM_TRAYICON: // 处理托盘消息
            if (lParam == WM_LBUTTONDBLCLK) { // 左键双击
                ShowWindow(hwnd, SW_RESTORE); // 恢复窗口
            } else if (lParam == WM_RBUTTONDOWN) { // 右键单击
                POINT pt;
                GetCursorPos(&pt); // 获取鼠标位置

                HMENU hMenu = CreatePopupMenu();
                AppendMenu(hMenu, MF_STRING | MF_BYPOSITION, 1, "Exit"); // 退出
                AppendMenu(hMenu, MF_STRING | MF_BYPOSITION, 2, "Open Settings"); // 设置
                AppendMenu(hMenu, MF_STRING | MF_BYPOSITION, 3, "About"); // 关于
                AppendMenu(hMenu, MF_STRING | MF_BYPOSITION, 4, "Show Current Settings"); // 显示当前设置

                SetForegroundWindow(hwnd); // 确保窗口在最上面
                TrackPopupMenu(hMenu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, hwnd, NULL); // 显示菜单
                DestroyMenu(hMenu); // 销毁菜单
            }
            return 0;

        case WM_COMMAND:
            if (LOWORD(wParam) == 1) { // 如果选择了退出
                Shell_NotifyIcon(NIM_DELETE, &nid); // 删除托盘图标
                DestroyWindow(hwnd); // 销毁窗口

                // 退出程序，确保线程终止
                ExitProcess(0);
            } else if (LOWORD(wParam) == 2) { // 如果选择了设置
                // 打开设置文件
                FILE *file = fopen("settings.txt", "r");
                if (file != NULL) {
                    char settingsBuffer[512];
                    snprintf(settingsBuffer, sizeof(settingsBuffer),
                             "The following are the keys bound to the current function:\n"
                             "Recoil Control: F1\n"
                             "Auto Click: F2\n"
                             "Save Settings: F5\n"
                             "Increase Y-offset: F7\n"
                             "Reduce Y-offset: F8\n"
                             "Increase X-offset: F9\n"
                             "Reduce X-offset: F10\n"
                             "Click the button below to view the settings file.",
                             keyRecoilControl, keyAutoClick, keySaveSettings,
                             keyIncreaseYOffset, keyReduceYOffset, keyIncreaseXOffset, keyReduceXOffset);

                    fclose(file);

                    // 显示消息框
                    MessageBox(hwnd, settingsBuffer, "The button bound to the current function", MB_OK);

                    // 打开设置文件
                    ShellExecute(NULL, "open", "settings.txt", NULL, NULL, SW_SHOWNORMAL);
                } else {
                    MessageBox(hwnd, "Unable to open settings file.", "Error", MB_OK | MB_ICONERROR);
                }
            } else if (LOWORD(wParam) == 3) { // 如果选择了关于
                MessageBox(
                    hwnd,
                    "TestKeyHook v14.0\n"
                    "Developed by yby360\n"
                    "Use F1 to enable/disable recoil control.\n"
                    "Use F2 to toggle auto-click feature.\n"
                    "Use F5 to save your current settings.\n"
                    "Adjust Y-axis and X-axis offsets using F7, F8, F9, and F10.\n"
                    "Press F12 to play or pause music.",
                    "About",
                    MB_OK
                );
            } else if (LOWORD(wParam) == 4) { // 显示当前设置
                char settingsBuffer[256];
                snprintf(settingsBuffer, sizeof(settingsBuffer),
                         "Recoil Control: %s\n"
                         "YOffset: %d\n"
                         "XOffset: %d\n"
                         "Bullet Count: %d\n"
                         "Fire Rate: %d\n"
                         "Recoil Delay: %d ms\n" // 显示当前延迟
                         "Trigger Mode: %s\n", // 新的触发模式
                         isRecoilControlActive ? "Enabled" : "Disabled",
                         yOffset,
                         xOffset,
                         bulletCount,
                         fireRate,
                         recoilDelay, // 新的 recoilDelay 显示
                         triggerMode ? "Both Mouse Buttons" : "Left Mouse Button"); // 更新触发模式显示
                MessageBox(hwnd, settingsBuffer, "Current Settings", MB_OK);
            }
            return 0;

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            // 创建一个带换行符的字符串
            char buffer[256];
            snprintf(buffer, sizeof(buffer),
                     "Recoil Control: %s\n"
                     "YOffset: %d\n"
                     "XOffset: %d\n"
                     "Bullet Count: %d\n"
                     "Fire Rate: %d\n"
                     "Recoil Delay: %d ms\n" // 显示当前延迟
                     "Trigger Mode: %s\n", // 新的触发模式
                     isRecoilControlActive ? "Enabled" : "Disabled",
                     yOffset,
                     xOffset,
                     bulletCount,
                     fireRate,
                     recoilDelay, // 新的 recoilDelay 显示
                     triggerMode ? "Both Mouse Buttons" : "Left Mouse Button"); // 更新触发模式显示

            // 使用 TextOut 逐行输出
            int startY = 10; // 起始 Y 坐标
            char *line = strtok(buffer, "\n"); // 获取每一行
            while (line != NULL) {
                TextOut(hdc, 10, startY, line, strlen(line)); // 输出当前行
                startY += 20; // 行间距20像素
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
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW, // 使窗口保持在最上面并设为工具窗口
        CLASS_NAME,
        "Current Parameters",
        WS_POPUP | WS_VISIBLE, // 使用弹出窗口样式以避免被截图
        CW_USEDEFAULT, CW_USEDEFAULT, 300, 300, // 更新窗口大小
        NULL, NULL, GetModuleHandle(NULL), NULL);

    // 创建托盘图标
    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = hwnd;
    nid.uID = 1; // 图标ID
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uCallbackMessage = WM_TRAYICON; // 将自定义消息与图标关联
    nid.hIcon = LoadIcon(NULL, IDI_APPLICATION); // 使用默认图标
    strcpy(nid.szTip, "Recoil Control Tool"); // 设置工具提示

    Shell_NotifyIcon(NIM_ADD, &nid); // 添加托盘图标

    // 修改窗口样式以防止截图（例如，无法捕获）
    LONG_PTR style = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
    style |= WS_EX_LAYERED; // 设置窗口为分层窗口
    SetWindowLongPtr(hwnd, GWL_EXSTYLE, style);
    SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA); // 设置透明度

    ShowWindow(hwnd, SW_SHOW);
}

// 更新GUI
void UpdateGui() {
    InvalidateRect(hwnd, NULL, TRUE); // 请求重绘窗口
}

// 播放音效的函数
void PlayEnableSound() {
    PlaySound(enableSoundPath, NULL, SND_FILENAME | SND_ASYNC);
}

void PlayDisableSound() {
    PlaySound(disableSoundPath, NULL, SND_FILENAME | SND_ASYNC);
}

// 切换触发模式的函数
void ToggleTriggerMode() {
    triggerMode = !triggerMode; // 切换触发模式
    SetConsoleColor(GREEN_COLOR);
    printf("Trigger mode switched to: %s\n", triggerMode ? "Both Mouse Buttons" : "Left Mouse Button");
    SetConsoleColor(DEFAULT_COLOR); // 恢复默认颜色
}

// 键盘监听线程函数
DWORD WINAPI KeyboardListenerThread(LPVOID lpParam) {
    while (1) {
        // 切换触发模式
        if (GetAsyncKeyState(VK_F11) & 0x8000) { // 假设使用F11切换触发模式
            ToggleTriggerMode();
            Sleep(300); // 防止重复触发
        }

        // 检查F1键是否被按下
        if (GetAsyncKeyState(keyRecoilControl) & 0x8000) {
            isRecoilControlActive = !isRecoilControlActive; // 切换后坐力控制状态
            SetConsoleColor(isRecoilControlActive ? GREEN_COLOR : RED_COLOR);
            printf("Recoil Control %s\n", isRecoilControlActive ? "Enabled" : "Disabled");
            SetConsoleColor(DEFAULT_COLOR); // 恢复默认颜色
            if (isRecoilControlActive) {
                PlayEnableSound();
            } else {
                PlayDisableSound();
            }
            Sleep(300); // 防止重复触发
        }

        // 检查F2键是否被按下
        if (GetAsyncKeyState(keyAutoClick) & 0x8000) {
            isAutoClickActive = !isAutoClickActive; // 切换自动点击功能状态
            SetConsoleColor(isAutoClickActive ? GREEN_COLOR : RED_COLOR);
            printf("Auto Click %s\n", isAutoClickActive ? "Enabled" : "Disabled");
            SetConsoleColor(DEFAULT_COLOR); // 恢复默认颜色
            if (isAutoClickActive) {
                PlayEnableSound();
            } else {
                PlayDisableSound();
            }
            Sleep(300); // 防止重复触发
        }

        // 检查F5键是否被按下
        if (GetAsyncKeyState(keySaveSettings) & 0x8000) {
            FILE *file = fopen("settings.txt", "w");
            if (file != NULL) {
                fprintf(file, "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",
                        yOffset, xOffset, bulletCount, fireRate,
                        keyRecoilControl, keyAutoClick, keySaveSettings,
                        keyIncreaseYOffset, keyReduceYOffset, keyIncreaseXOffset, keyReduceXOffset,
                        autoClickDelayBefore, autoClickDelayAfter, clickCount, recoilResetMultiplier,
                        recoilDelay); // 新的 recoilDelay 添加
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

        // 检查F7键是否被按下
        if (GetAsyncKeyState(keyIncreaseYOffset) & 0x8000) {
            yOffset += 1; // 增加Y轴偏移
            SetConsoleColor(BLUE_COLOR);
            printf("YOffset increased to: %d\n", yOffset);
            SetConsoleColor(DEFAULT_COLOR); // 恢复默认颜色
            Sleep(300); // 防止重复触发
        }

        // 检查F8键是否被按下
        if (GetAsyncKeyState(keyReduceYOffset) & 0x8000) {
            yOffset -= 1; // 减少Y轴偏移
            SetConsoleColor(BLUE_COLOR);
            printf("YOffset decreased to: %d\n", yOffset);
            SetConsoleColor(DEFAULT_COLOR); // 恢复默认颜色
            Sleep(300); // 防止重复触发
        }

        // 检查F9键是否被按下
        if (GetAsyncKeyState(keyIncreaseXOffset) & 0x8000) {
            xOffset += 1; // 增加X轴偏移
            SetConsoleColor(BLUE_COLOR);
            printf("XOffset increased to: %d\n", xOffset);
            SetConsoleColor(DEFAULT_COLOR); // 恢复默认颜色
            Sleep(300); // 防止重复触发
        }

        // 检查F10键是否被按下
        if (GetAsyncKeyState(keyReduceXOffset) & 0x8000) {
            xOffset -= 1; // 减少X轴偏移
            SetConsoleColor(BLUE_COLOR);
            printf("XOffset decreased to: %d\n", xOffset);
            SetConsoleColor(DEFAULT_COLOR); // 恢复默认颜色
            Sleep(300); // 防止重复触发
        }

        // 检查F12键是否被按下
        if (GetAsyncKeyState(VK_F12) & 0x8000) {
            SetConsoleColor(isMusicPlaying ? RED_COLOR : GREEN_COLOR);
            if (isMusicPlaying) {
                PlaySound(NULL, 0, 0); // 停止音乐播放
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

// 后坐力控制线程函数
DWORD WINAPI RecoilControlThread(LPVOID lpParam) {
    bool wasFiring = false; // 新变量以指示发射状态
    DWORD startTime; // 变量记录开始时间

    while (1) {
        // 根据触发模式检查条件
        bool leftButtonPressed = GetAsyncKeyState(VK_LBUTTON) & 0x8000;
        bool rightButtonPressed = GetAsyncKeyState(VK_RBUTTON) & 0x8000;

        if (triggerMode == 0) { // 左键模式
            if (isRecoilControlActive && leftButtonPressed) {
                if (!wasFiring) {
                    startTime = GetTickCount(); // 记录开始时间以进行后坐力控制
                }

                printf("VK_LBUTTON_RUN\n");

                int currentYOffset = yOffset; // 默认全后坐力控制
                if (!(rightButtonPressed)) {
                    // 如果没有按下右键，视为非瞄准模式，后坐力掉落为一半
                    currentYOffset /= 2;
                }

                for (int i = 0; i < bulletCount; i++) { // 调整循环计数和移动，以适应不同游戏和武器
                    int randomX;
                    // 检查是否在 recoilDelay 毫秒内
                    if (GetTickCount() - startTime < recoilDelay) {
                        randomX = 0; // 在前 recoilDelay 毫秒内，随机值为0
                    } else {
                        randomX = (rand() % (2 * xOffset + 1)) - xOffset; // 生成- xOffset 到 xOffset 之间的随机数
                    }
                    mouse_event(MOUSEEVENTF_MOVE, randomX, currentYOffset, 0, 0);
                    Sleep(fireRate); // 根据不同射速调整移动间隔
                }
                wasFiring = true; // 记录当前正在发射
            } else if (isRecoilControlActive && !leftButtonPressed && wasFiring) {
                // 当左键释放且之前正在发射时，自动抬起鼠标指针
                printf("VK_LBUTTON_RUN_RESET\n");
                mouse_event(MOUSEEVENTF_MOVE, 0, -recoilResetMultiplier * yOffset, 0, 0); // 抬起鼠标指针
                wasFiring = false; // 重置发射状态
            }
        } else if (triggerMode == 1) { // 同时按下模式
            if (isRecoilControlActive && leftButtonPressed && rightButtonPressed) {
                if (!wasFiring) {
                    startTime = GetTickCount(); // 记录开始时间以进行后坐力控制
                }

                printf("VK_BOTH_BUTTONS_RUN\n");

                int currentYOffset = yOffset; // 默认全后坐力控制

                for (int i = 0; i < bulletCount; i++) { // 调整循环计数和移动，以适应不同游戏和武器
                    int randomX;
                    // 检查是否在 recoilDelay 毫秒内
                    if (GetTickCount() - startTime < recoilDelay) {
                        randomX = 0; // 在前 recoilDelay 毫秒内，随机值为0
                    } else {
                        randomX = (rand() % (2 * xOffset + 1)) - xOffset; // 生成 -xOffset 到 xOffset 之间的随机数
                    }
                    mouse_event(MOUSEEVENTF_MOVE, randomX, currentYOffset, 0, 0);
                    Sleep(fireRate); // 根据不同射速调整移动间隔
                }
                wasFiring = true; // 记录当前正在发射
            } else if (isRecoilControlActive && !(leftButtonPressed && rightButtonPressed) && wasFiring) {
                // 当左右键都释放且之前正在发射时，自动抬起鼠标指针
                printf("VK_BOTH_BUTTONS_RUN_RESET\n");
                mouse_event(MOUSEEVENTF_MOVE, 0, -recoilResetMultiplier * yOffset, 0, 0); // 抬起鼠标指针
                wasFiring = false; // 重置发射状态
            }
        }
        Sleep(5); // 防止重复触发
    }
    return 0;
}

// 自动点击线程函数
DWORD WINAPI AutoClickThread(LPVOID lpParam) {
    while (1) {
        // 根据触发模式检查条件
        bool leftButtonPressed = GetAsyncKeyState(VK_LBUTTON) & 0x8000;
        bool rightButtonPressed = GetAsyncKeyState(VK_RBUTTON) & 0x8000;

        if (triggerMode == 0) { // 左键模式
            if (isAutoClickActive && leftButtonPressed) {
                // 等待左键释放
                while (leftButtonPressed) {
                    Sleep(5); // 短暂休眠以避免高CPU使用
                    leftButtonPressed = GetAsyncKeyState(VK_LBUTTON) & 0x8000; // 重新检查状态
                }

                int currentClickCount = clickCount; // 使用配置的点击计数

                while (currentClickCount > 0) {
                    Sleep(autoClickDelayBefore);
                    printf("AutoClickRUN\n");
                    mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
                    Sleep(autoClickDelayAfter);
                    mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
                    currentClickCount--;
                }
            }
        } else if (triggerMode == 1) { // 同时按下模式
            if (isAutoClickActive && leftButtonPressed && rightButtonPressed) {
                // 等待左键和右键都释放
                while (leftButtonPressed && rightButtonPressed) {
                    Sleep(5); // 短暂休眠以避免高CPU使用
                    leftButtonPressed = GetAsyncKeyState(VK_LBUTTON) & 0x8000; // 重新检查状态
                    rightButtonPressed = GetAsyncKeyState(VK_RBUTTON) & 0x8000; // 重新检查状态
                }

                int currentClickCount = clickCount; // 使用配置的点击计数

                while (currentClickCount > 0) {
                    Sleep(autoClickDelayBefore);
                    printf("AutoClickRUN\n");
                    mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
                    Sleep(autoClickDelayAfter);
                    mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
                    currentClickCount--;
                }
            }
        }
        Sleep(5); // 防止重复触发
    }
}

JNIEXPORT void JNICALL Java_GlobalKeyListener2_startListening(JNIEnv *env, jobject obj) {
    SetConsoleColor(BLUE_COLOR);
    printf("Instructions:\n");
    printf("F1\t: Enable/Disable recoil control\n");
    printf("F2\t: Enable/Disable auto-click feature\n");
    printf("F5\t: Save current settings\n");
    printf("F7\t: Increase Y-axis offset\n");
    printf("F8\t: Decrease Y-axis offset\n");
    printf("F9\t: Increase X-axis offset\n");
    printf("F10\t: Decrease X-axis offset\n");
    printf("F11\t: Toggle trigger mode (Left Click/Both Buttons)\n");
    printf("F12\t: Play/Pause music\n");
    printf("Left-click to start using\n");
    printf("Developer: yby360\n");
    SetConsoleColor(DEFAULT_COLOR); // 恢复默认颜色

    SetConsoleColor(BLUE_COLOR);
    printf("Starting parameter GUI...\n");
    CreateGuiWindow(); // 创建GUI窗口

    // 尝试从文件读取设置
    FILE *file = fopen("settings.txt", "r");
    if (file != NULL) {
        fscanf(file, "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d",
               &yOffset, &xOffset, &bulletCount, &fireRate,
               &keyRecoilControl, &keyAutoClick, &keySaveSettings,
               &keyIncreaseYOffset, &keyReduceYOffset, &keyIncreaseXOffset, &keyReduceXOffset,
               &autoClickDelayBefore, &autoClickDelayAfter, &clickCount, &recoilResetMultiplier,
               &recoilDelay); // 新的 recoilDelay 添加
        fclose(file);
        SetConsoleColor(GREEN_COLOR);
        printf("Loaded settings: YOffset = %d, XOffset = %d, BulletCount = %d, FireRate = %d\n", yOffset, xOffset, bulletCount, fireRate);
        printf("Loaded key settings: RecoilControl = %d, AutoClick = %d, SaveSettings = %d, IncreaseYOffset = %d, ReduceYOffset = %d, IncreaseXOffset = %d, ReduceXOffset = %d\n",
               keyRecoilControl, keyAutoClick, keySaveSettings, keyIncreaseYOffset, keyReduceYOffset, keyIncreaseXOffset, keyReduceXOffset);
        printf("Loaded auto-click settings: DelayBefore = %d, DelayAfter = %d, ClickCount = %d\n", autoClickDelayBefore, autoClickDelayAfter, clickCount);
        printf("Loaded recoil reset multiplier: %d\n", recoilResetMultiplier);
        printf("Loaded recoil delay: %d ms\n", recoilDelay); // 新的 recoilDelay 显示
        SetConsoleColor(DEFAULT_COLOR); // 恢复默认颜色
    } else {
        SetConsoleColor(RED_COLOR);
        printf("Settings file not found, using default values.\n"); // 未找到设置文件，使用默认值
        SetConsoleColor(DEFAULT_COLOR); // 恢复默认颜色
    }

    // 初始化随机数生成器
    srand(time(NULL));

    // 创建键盘监听线程
    HANDLE keyboardThread = CreateThread(NULL, 0, KeyboardListenerThread, NULL, 0, NULL);
    if (keyboardThread == NULL) {
        SetConsoleColor(RED_COLOR);
        printf("Failed to create keyboard listener thread.\n"); // 创建键盘监听线程失败
        SetConsoleColor(DEFAULT_COLOR); // 恢复默认颜色
        return;
    }

    // 创建后坐力控制线程
    HANDLE recoilThread = CreateThread(NULL, 0, RecoilControlThread, NULL, 0, NULL);
    if (recoilThread == NULL) {
        SetConsoleColor(RED_COLOR);
        printf("Failed to create recoil control thread.\n"); // 创建后坐力控制线程失败
        SetConsoleColor(DEFAULT_COLOR); // 恢复默认颜色
        return;
    }

    // 创建自动点击线程
    HANDLE autoClickThread = CreateThread(NULL, 0, AutoClickThread, NULL, 0, NULL);
    if (autoClickThread == NULL) {
        SetConsoleColor(RED_COLOR);
        printf("Failed to create auto-click thread.\n"); // 创建自动点击线程失败
        SetConsoleColor(DEFAULT_COLOR); // 恢复默认颜色
        return;
    }

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // 等待线程结束（实际上这些线程是无限循环，因此不会结束）
    WaitForSingleObject(keyboardThread, INFINITE);
    WaitForSingleObject(recoilThread, INFINITE);
    WaitForSingleObject(autoClickThread, INFINITE);
}
