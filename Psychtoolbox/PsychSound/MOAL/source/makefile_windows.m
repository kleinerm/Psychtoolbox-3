function makefile_windows
% Makefile for building the moalcore.dll MEX file for Matlab+OpenAL under
% M$-Windows. Call it while inside the .../MOAL/source folder. You'll
% have to copy the OpenAL32.lib and the header files from the Creative
% labs OpenAL SDK into the include/ and libs/ folder of you Visual Studio
% installation for this to work.

mex -v -outdir C:\kleinerm\trunk\Psychtoolbox\PsychSound\MOAL\source\ -output moalcore -IC:\Programme\MicrosoftVisualStudio\VC98\Include -IC:\kleinerm\trunk\Psychtoolbox\PsychSound\MOAL\source -DWINDOWS -DBUILD_GLM windowhacks.c al_auto.c al_manual.c moalcore.c alm.c user32.lib openal32.lib
dos('copy C:\kleinerm\trunk\Psychtoolbox\PsychSound\MOAL\source\moalcore.dll C:\kleinerm\trunk\Psychtoolbox\PsychSound\MOAL\core\');
return;
