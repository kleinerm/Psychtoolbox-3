function makefile_windows
mex -v -outdir T:\projects\OpenGLPsychtoolbox\trunk\Psychtoolbox\PsychOpenGL\MOGL\source\ -output moglcore -IC:\Programme\MicrosoftVisualStudio\VC98\Include -IT:\projects\OpenGLPsychtoolbox\trunk\Psychtoolbox\PsychOpenGL\MOGL\source -DMATLABR11 -DWINDOWS -DGLEW_STATIC windowhacks.c gl_auto.c gl_manual.c mogl_rebinder.c moglcore.c glew.c user32.lib gdi32.lib advapi32.lib glu32.lib opengl32.lib glut32.lib
dos('copy T:\projects\OpenGLPsychtoolbox\trunk\Psychtoolbox\PsychOpenGL\MOGL\source\moglcore.dll T:\projects\OpenGLPsychtoolbox\trunk\Psychtoolbox\PsychBasic\MatlabWindowsFilesR11\');
return;

