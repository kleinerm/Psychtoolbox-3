function makefile_windows_r2007a
mex -v -outdir U:\projects\OpenGLPsychtoolbox\trunk\Psychtoolbox\PsychOpenGL\MOGL\source\ -output moglcore -DTARGET_OS_WIN32 -I"C:\Programme\Microsoft Visual Studio 8\VC\Include" -IU:\projects\OpenGLPsychtoolbox\trunk\Psychtoolbox\PsychOpenGL\MOGL\source -DWINDOWS -DGLEW_STATIC windowhacks.c gl_auto.c gl_manual.c mogl_rebinder.c moglcore.c glew.c user32.lib gdi32.lib advapi32.lib glu32.lib opengl32.lib glut32.lib
dos('copy U:\projects\OpenGLPsychtoolbox\trunk\Psychtoolbox\PsychOpenGL\MOGL\source\moglcore.mexw32 U:\projects\OpenGLPsychtoolbox\trunk\Psychtoolbox\PsychBasic\MatlabWindowsFilesR2007a\');
return;

