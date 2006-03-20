function makefile_windows
mex -v -outdir C:\kleinerm\trunk\Psychtoolbox\PsychOpenGL\MOGL\source\ -output moglcore -IC:\Programme\MicrosoftVisualStudio\VC98\Include -IC:\kleinerm\trunk\Psychtoolbox\PsychOpenGL\MOGL\source -DWINDOWS -DGLEW_STATIC windowhacks.c gl_auto.c gl_manual.c moglcore.c glew.c user32.lib gdi32.lib advapi32.lib glu32.lib opengl32.lib glut32.lib
dos('copy C:\kleinerm\trunk\Psychtoolbox\PsychOpenGL\MOGL\source\moglcore.dll C:\kleinerm\trunk\Psychtoolbox\PsychOpenGL\MOGL\core\');
return;

