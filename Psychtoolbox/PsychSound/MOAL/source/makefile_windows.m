function makefile_windows(postR2007a)
% Makefile for building the moalcore.dll MEX file for Matlab+OpenAL under
% M$-Windows. Call it while inside the .../MOAL/source folder. You'll
% have to copy the OpenAL32.lib and the header files from the Creative
% labs OpenAL SDK into the include/ and libs/ folder of you Visual Studio
% installation for this to work.

if nargin < 1
    postR2007a = [];
end

if isempty(postR2007a)
    postR2007a = 1;
end

if postR2007a
    % R2007a or later build:
    mex -v -outdir T:\projects\OpenGLPsychtoolbox\trunk\PsychSourceGL\Projects\Windows\build\ -output moalcore -I"C:\Programme\Microsoft Visual Studio 8\VC\Include" -IT:\projects\OpenGLPsychtoolbox\trunk\Psychtoolbox\PsychSound\MOAL\source -DWINDOWS windowhacks.c al_auto.c al_manual.c moalcore.c alm.c user32.lib openal32.lib
    dos('copy T:\projects\OpenGLPsychtoolbox\Psychtoolbox-3\PsychSourceGL\Projects\Windows\build\moalcore.mexw32 T:\projects\OpenGLPsychtoolbox\Psychtoolbox-3\Psychtoolbox\PsychBasic\MatlabWindowsFilesR2007a\');
else
    % Pre R2007a build: Unsupported:
    error('Building moalcore on pre R2007a Matlab is no longer supported.');
end

return;
