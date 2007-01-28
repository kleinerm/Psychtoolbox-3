function windowsmakeeyelink
% windowsmakeeyelink -- Simple build-script for building
% Eyelink toolbox for M$-Windows:
% You will need to adapt the paths for your build system manually.

% Copy our C++ PsychScriptingGlue.cc to a C style PsychScriptingGlue.c so the f%*§$!d M$-Compiler
% can handle it:
dos('copy C:\kleinerm\trunk\PsychSourceGL\Source\Common\Base\PsychScriptingGlue.cc C:\kleinerm\trunk\PsychSourceGL\Source\Common\Base\PsychScriptingGlue.c');

% Build sequence for Eyelink.dll: Requires the freely downloadable (after registration) Eyelink-SDK for Windows.
mex -v -outdir C:\kleinerm\trunk\PsychSourceGL\Projects\Windows\build\ -output Eyelink -IC:\Programme\SRResearch\EyeLink\Includes\eyelink -IC:\Programme\QuickTimeSDK\CIncludes -IC:\Programme\MicrosoftVisualStudio\VC98\Include -ICommon\Base -ICommon\Eyelink -IWindows\Base Windows\Base\*.c Common\Base\*.c Common\Eyelink\*.c user32.lib gdi32.lib advapi32.lib winmm.lib C:\Programme\SRResearch\EyeLink\libs\eyelink_core.lib C:\Programme\SRResearch\EyeLink\libs\eyelink_w32_comp.lib C:\Programme\SRResearch\EyeLink\libs\eyelink_exptkit20.lib

% Move Eyelink.dll into its proper location:
dos('copy C:\kleinerm\trunk\PsychSourceGL\Projects\Windows\build\Eyelink.dll C:\kleinerm\trunk\Psychtoolbox\PsychHardware\EyelinkToolbox\EyelinkBasic\');

% Delete the temporary .c version of Scripting Glue:
delete('C:\kleinerm\trunk\PsychSourceGL\Source\Common\Base\PsychScriptingGlue.c');
return;
