cd F:\modtest\TestKEYHook\src
del GlobalKeyListener2.dll
gcc -shared -o GlobalKeyListener2.dll -I"%JAVA_HOME%\include" -I"%JAVA_HOME%\include\win32" GlobalKeyListener2.c -lwinmm -lgdi32