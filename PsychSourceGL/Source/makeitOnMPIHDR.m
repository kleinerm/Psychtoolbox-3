function makeitOnMPIHDR(what)

dos('copy U:\projects\OpenGLPsychtoolbox\trunk\PsychSourceGL\Source\Common\Base\PsychScriptingGlue.cc U:\projects\OpenGLPsychtoolbox\trunk\PsychSourceGL\Source\Common\Base\PsychScriptingGlue.c');

if nargin < 1
   what = 0;
end

if what == 0
   % Default: Build Screen.dll
   mex -v -outdir U:\projects\OpenGLPsychtoolbox\trunk\PsychSourceGL\Projects\Windows\build\ -output Screen.dll -DPTBMODULE_Screen -DTARGET_OS_WIN32 -ID:\install\QuickTimeSDK\CIncludes -I"C:\Programme\Microsoft Visual Studio 8\VC\Include" -I"C:\Program Files\Microsoft DirectX SDK\Include" -ICommon\Base -ICommon\Screen -IWindows\Base -IWindows\Screen Windows\Screen\*.c Windows\Base\*.c Common\Base\*.c Common\Screen\*.c user32.lib gdi32.lib advapi32.lib glu32.lib opengl32.lib qtmlClient.lib ddraw.lib winmm.lib  
   dos('copy U:\projects\OpenGLPsychtoolbox\trunk\PsychSourceGL\Projects\Windows\build\Screen.dll U:\projects\OpenGLPsychtoolbox\trunk\Psychtoolbox\PsychBasic\');
end

if what == 1
   % Build WaitSecs.dll
   mex -v -outdir U:\projects\OpenGLPsychtoolbox\trunk\PsychSourceGL\Projects\Windows\build\ -output WaitSecs.dll -DPTBMODULE_WaitSecs -I"C:\Programme\Microsoft Visual Studio 8\VC\Include" -ICommon\Base -ICommon\WaitSecs -IWindows\Base Windows\Base\*.c Common\Base\*.c Common\WaitSecs\*.c user32.lib winmm.lib
   dos('copy U:\projects\OpenGLPsychtoolbox\trunk\PsychSourceGL\Projects\Windows\build\WaitSecs.dll U:\projects\OpenGLPsychtoolbox\trunk\Psychtoolbox\PsychBasic\');
end

% TODO
if what == 2
   % Build PsychPortAudio.dll
   mex -v -outdir U:\projects\OpenGLPsychtoolbox\trunk\PsychSourceGL\Projects\Windows\build\ -output PsychPortAudio -DPTBMODULE_PsychPortAudio -IC:\Programme\MicrosoftVisualStudio\VC98\Include -ICommon\Base -ICommon\PsychPortAudio -IWindows\Base Windows\Base\*.c Common\Base\*.c Common\PsychPortAudio\*.c user32.lib winmm.lib portaudio_x86.lib
   dos('copy U:\projects\OpenGLPsychtoolbox\trunk\PsychSourceGL\Projects\Windows\build\PsychPortAudio.dll U:\projects\OpenGLPsychtoolbox\trunk\Psychtoolbox\PsychBasic\');
end

if what == 3
   % Build GetSecs.dll
   mex -v -outdir U:\projects\OpenGLPsychtoolbox\trunk\PsychSourceGL\Projects\Windows\build\ -output GetSecs.dll -DPTBMODULE_GetSecs -I"C:\Programme\Microsoft Visual Studio 8\VC\Include" -ICommon\Base -ICommon\GetSecs -IWindows\Base Windows\Base\*.c Common\Base\*.c Common\GetSecs\*.c user32.lib winmm.lib
   dos('copy U:\projects\OpenGLPsychtoolbox\trunk\PsychSourceGL\Projects\Windows\build\GetSecs.dll U:\projects\OpenGLPsychtoolbox\trunk\Psychtoolbox\PsychBasic\');
end

delete('U:\projects\OpenGLPsychtoolbox\trunk\PsychSourceGL\Source\Common\Base\PsychScriptingGlue.c');
return;
