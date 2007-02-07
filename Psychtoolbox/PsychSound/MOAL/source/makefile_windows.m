function makefile_windows
mex -v -outdir C:\kleinerm\trunk\Psychtoolbox\PsychSound\MOAL\source\ -output moalcore -IC:\Programme\MicrosoftVisualStudio\VC98\Include -IC:\kleinerm\trunk\Psychtoolbox\PsychSound\MOAL\source -DWINDOWS -DBUILD_GLM windowhacks.c al_auto.c al_manual.c moalcore.c alm.c user32.lib gdi32.lib advapi32.lib openal32.lib
dos('copy C:\kleinerm\trunk\Psychtoolbox\PsychSound\MOAL\source\moalcore.dll C:\kleinerm\trunk\Psychtoolbox\PsychSound\MOAL\core\');
return;
