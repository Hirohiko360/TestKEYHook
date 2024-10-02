# TestKEYHook
TestKEYHook一款简易通用的鼠标下压及其扩展软件

注意！这是测试版文档，需要使用稳定版的exe文件请切换分支或者在发布中寻找对应版本

使用教程：用记事本打开TestKEYHook.bat
修改 cd F:\modtest\TestKEYHook\src为cd 本文件夹根目录
双击TestKEYHook.bat打开窗口即可
（您至少需要安装java和配置环境变量）

开发教程：下载源码，您至少需要安装java和配置环境变量
确保你的计算机上已安装好C语言编译环境（如GCC）
修改代码并使用build.bat运行

# 热键说明
F1：启用/禁用后坐力控制。
F2：启用/禁用自动点击功能。
F5：保存当前设置到文件。
F7：增加Y轴偏移量。
F8：减少Y轴偏移量。
F9：增加X轴偏移量。
F10：减少X轴偏移量。
F11：切换触发模式（左键点击或同时按下两个按钮）。
F12：播放/暂停音乐。

# 默认打开
1.cmd输出窗口
2.配置信息gui
3.托盘程序

# 使用技巧
调整Y轴和X轴的偏移：在射击中，适当调整Y轴和X轴的偏移量可以帮助你更好地控制武器的后坐力，适合不同类型的武器。
切换触发模式：使用F11键切换触发模式，选择适合当前游戏的模式，左键模式更适合单发武器，双按钮模式适合连发武器。

# 自定义按键及参数
settings.txt中进行更改，更改后请打开程序查看cmd输出读取的配置
#关于自定义按键
按键名称和对应数值
以下是按键名称和对应的数值表：

映射表示例: | 按键名称 | 按键值 | 
| F1 | 112 | | F2 | 113 | | F3 | 114 | | F4 | 115 | | F5 | 116 | | F6 | 117 | | F7 | 118 | | F8 | 119 | | F9 | 120 | | F10 | 121 | | F11 | 122 | | F12 | 123 | | 左键 (LBUTTON) | 1 | | 右键 (RBUTTON) | 2 | | 中键 (MBUTTON) | 4 | | 鼠标侧键1 (XBUTTON1) | 5 | | 鼠标侧键2 (XBUTTON2) | 6 | | 回车键 (RETURN) | 13 | | 空格键 (SPACE) | 32 | | 左箭头 (LEFT) | 37 | | 上箭头 (UP) | 38 | | 右箭头 (RIGHT) | 39 | | 下箭头 (DOWN) | 40 | | A | 65 | | B | 66 | | C | 67 | | D | 68 | | E | 69 | | F | 70 | | G | 71 | | H | 72 | | I | 73 | | J | 74 | | K | 75 | | L | 76 | | M | 77 | | N | 78 | | O | 79 | | P | 80 | | Q | 81 | | R | 82 | | S | 83 | | T | 84 | | U | 85 | | V | 86 | | W | 87 | | X | 88 | | Y | 89 | | Z | 90 |

特殊：
1.左键模式下的操作：仅按下左键：后坐力效果将减半（如默认Y轴偏移3变为1.5），适合不精确瞄准的射击。

2.程序默认会在500毫秒的延迟前不会有鼠标抖动（随机X偏移为0），确保用户稳定瞄准，可更改。

#故障排除
无法加载设置：确保settings.txt文件位于程序目录下。如果在第一次运行时未找到该文件，程序将使用默认值。
热键无效：确保程序具有足够的权限来监听全局键盘事件，尝试以管理员身份运行程序。
技术支持
如有其他问题或功能需求，可以联系开发者：yby360。

# TestKEYHook
TestKEYHook is a simple and universal software for mouse down pressing and its extension

be careful! This is a beta version document. If you need to use a stable version of the exe file, please switch branches or search for the corresponding version in the release

Usage tutorial: Open TestKEYHook.bat with Notepad
Modify cd F: \ modtest \ TestKEYHook \ src to the root directory of the cd folder
Double click TestKEYHook.bat to open the window
(You need to install Java and configure environment variables at least)

Development tutorial: Download source code, you need to install Java and configure environment variables at least
Ensure that the C language compilation environment (such as GCC) is installed on your computer
Modify the code and run it using build.bat

#Hot key description
F1: Enable/disable recoil control.
F2: Enable/disable the automatic click function.
F5: Save the current settings to a file.
F7: Increase the Y-axis offset.
F8: Reduce Y-axis offset.
F9: Increase the X-axis offset.
F10: Reduce the X-axis offset.
F11: Switch trigger mode (left click or press both buttons simultaneously).
F12: Play/pause music.

#Default Open
1. cmd output window
2. Configure information GUI
3. Tray program

#Usage skills
Adjusting the offset of the Y-axis and X-axis: In shooting, adjusting the offset of the Y-axis and X-axis appropriately can help you better control the recoil of the weapon, suitable for different types of weapons.
Switch trigger mode: Use the F11 key to switch trigger modes and select the mode that is suitable for the current game. Left button mode is more suitable for single shot weapons, while dual button mode is suitable for continuous shot weapons.

#Customize buttons and parameters
Make changes in the settings. txt file. After the changes, please open the program to view the configuration read from cmd output
#About customizing buttons
Key name and corresponding numerical value
The following is a table of key names and corresponding numerical values:

Mapping representation example: | Key name | Key value|
|F1 | 112 | | F2 | 113 | F3 | 114 | | F4 | 115 | | F5 | 116 | F6 | 117 | | F7 | 118 | F8 | 119 | | F9 | 120 | F10 | 121 | | F11 | 122 | F12 | 123 | | Left button (LBUTTON) | 1 | Right button (RBUTTON) | 2 | Middle button (MBUTTON) | 4 | Mouse side key 1 (XBUTTON1) | 5 | Mouse side key 2 (XBUTTON2) | 6 | Enter key (RETURN) | 13 | Space bar (SPACE) | 32 | | Left arrow LEFT | 37 | | UP | 38 | | RIGHT | 39 | | DOWN | 40 | A | 65 | B | 66 | C | 67 | | D | 68 | E | 69 | F | 70 | G | 71 | H | 72 | | I | 73 | J | 74 | K | 75 | L | 76 | M | 77 | N | 78 | O | 79 | P | 80 | Q | 81 | R | 82 | S | 83 | | T | 84 | U | 85 | V | 86 | | W | 87 | | X | 88 | | Y | 89 | | Z | 90|

Special:
1. Operation in left button mode: Pressing only the left button: The recoil effect will be halved (such as the default Y-axis offset of 3 becoming 1.5), suitable for shooting with imprecise aiming.

2. The program defaults to no mouse shake before a delay of 500 milliseconds (with a random X offset of 0) to ensure stable aiming for the user, which can be changed.
