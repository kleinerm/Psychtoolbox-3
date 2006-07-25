function makeit
dos('copy C:\kleinerm\trunk\PsychSourceGL\Source\Common\Base\PsychScriptingGlue.cc C:\kleinerm\trunk\PsychSourceGL\Source\Common\Base\PsychScriptingGlue.c');
mex -v -outdir C:\kleinerm\trunk\PsychSourceGL\Projects\Windows\build\ -output Screen -IC:\Programme\QuickTimeSDK\CIncludes -IC:\Programme\MicrosoftVisualStudio\VC98\Include -ICommon\Base -ICommon\Screen -IWindows\Base -IWindows\Screen Windows\Screen\*.c Windows\Base\*.c Common\Base\*.c Common\Screen\*.c user32.lib gdi32.lib advapi32.lib glu32.lib opengl32.lib qtmlClient.lib
dos('copy C:\kleinerm\trunk\PsychSourceGL\Projects\Windows\build\Screen.dll C:\kleinerm\trunk\Psychtoolbox\PsychBasic\');
dos('copy C:\kleinerm\trunk\PsychSourceGL\Projects\Windows\build\Screen.dll C:\MATLABR11\toolbox\PsychToolbox\PsychBasic\');
delete('C:\kleinerm\trunk\PsychSourceGL\Source\Common\Base\PsychScriptingGlue.c');
return;
