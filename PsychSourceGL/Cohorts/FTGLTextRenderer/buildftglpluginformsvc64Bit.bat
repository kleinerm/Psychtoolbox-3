rem **** Build sequence for libptbdrawtext_ftgl64.dll our Matlab 64-Bit text renderer plugin for Windows ****
rem **** This is for building the DLL for Matlab only. Requires 64-Bit GStreamer 1.x SDK installed.      ****

rem **** Set pathes to the MSVC build tools and Windows 7 SDK ****
set VSINSTALLDIR=C:\Program Files (x86)\Microsoft Visual Studio 10.0
set VCINSTALLDIR=%VSINSTALLDIR%\VC
set LINKERDIR=C:\Program Files\Microsoft SDKs\Windows\v7.1\
set PATH=%VCINSTALLDIR%\bin\amd64;%VCINSTALLDIR%\bin;%VCINSTALLDIR%\VCPackages;%VSINSTALLDIR%\Common7\IDE;%VSINSTALLDIR%\Common7\Tools;%LINKERDIR%\bin\x64;%LINKERDIR%\bin;%PATH%
set INCLUDE=%VCINSTALLDIR%\INCLUDE;%LINKERDIR%\include;%LINKERDIR%\include\gl;%VCINSTALLDIR%\ATLMFC\INCLUDE;%INCLUDE%
set LIB=%VCINSTALLDIR%\LIB\amd64;%LINKERDIR%\LIB\X64;%VCINSTALLDIR%\ATLMFC\LIB\AMD64;%LIB%

cl /c /GR /W3 /EHs /D_CRT_SECURE_NO_DEPRECATE /D_SCL_SECURE_NO_DEPRECATE /D_SECURE_SCL=0 /nologo /MD /I"C:\gstreamer\1.0\x86_64\include\freetype2" /I"C:\gstreamer\1.0\x86_64\include" /Foqstringqcharemulation.obj /O2 /Oy- /DNDEBUG -DOGLFT_BUILD qstringqcharemulation.cpp 
cl /c /GR /W3 /EHs /D_CRT_SECURE_NO_DEPRECATE /D_SCL_SECURE_NO_DEPRECATE /D_SECURE_SCL=0 /nologo /MD /I"C:\gstreamer\1.0\x86_64\include\freetype2" /I"C:\gstreamer\1.0\x86_64\include" /Folibptbdrawtext_ftgl.obj /O2 /Oy- /DNDEBUG -DOGLFT_BUILD libptbdrawtext_ftgl.cpp 
cl /c /GR /W3 /EHs /D_CRT_SECURE_NO_DEPRECATE /D_SCL_SECURE_NO_DEPRECATE /D_SECURE_SCL=0 /nologo /MD /I"C:\gstreamer\1.0\x86_64\include\freetype2" /I"C:\gstreamer\1.0\x86_64\include" /FoOGLFT.obj /O2 /Oy- /DNDEBUG -DOGLFT_BUILD OGLFT.cpp 
link /out:".\libptbdrawtext_ftgl64.dll" /dll /LIBPATH:"C:\gstreamer\1.0\x86_64\lib" fontconfig.lib freetype.lib opengl32.lib glu32.lib /MACHINE:X64 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib qstringqcharemulation.obj libptbdrawtext_ftgl.obj OGLFT.obj /nologo /manifest /incremental:NO
del *.obj
del libptbdrawtext_ftgl64.lib
del libptbdrawtext_ftgl64.exp
del libptbdrawtext_ftgl64.dll.manifest
