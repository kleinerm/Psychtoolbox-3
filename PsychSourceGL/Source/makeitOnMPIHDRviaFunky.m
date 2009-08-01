function makeitOnMPIHDRviaFunky(what, onoctave)

dos('copy T:\projects\OpenGLPsychtoolbox\trunk\PsychSourceGL\Source\Common\Base\PsychScriptingGlue.cc T:\projects\OpenGLPsychtoolbox\trunk\PsychSourceGL\Source\Common\Base\PsychScriptingGlue.c');

if nargin < 1
   what = 0;
end

if nargin < 2
    onoctave = 0;
end

% Matlab or Octave build?
if onoctave == 0
    % Matlab build:
    if what == 0
        % Default: Build Screen.mexw32
        mex -v -outdir T:\projects\OpenGLPsychtoolbox\trunk\PsychSourceGL\Projects\Windows\build\ -output Screen.mexw32 -DPTBMODULE_Screen -DPTBVIDEOCAPTURE_ARVIDEO -DPTBVIDEOCAPTURE_QT -DTARGET_OS_WIN32 -ID:\install\QuickTimeSDK\CIncludes -I"C:\Programme\Microsoft Visual Studio 8\VC\Include" -I"D:\MicrosoftDirectXSDK\Include" -ICommon\Base -ICommon\Screen -IWindows\Base -IWindows\Screen -I..\Cohorts\ARToolkit\include Windows\Screen\*.c Windows\Base\*.c Common\Base\*.c Common\Screen\*.c kernel32.lib user32.lib gdi32.lib advapi32.lib glu32.lib opengl32.lib qtmlClient.lib ddraw.lib winmm.lib delayimp.lib libARvideo.lib LINKFLAGS="$LINKFLAGS /DELAYLOAD:libARvideo.dll"
        dos('copy T:\projects\OpenGLPsychtoolbox\trunk\PsychSourceGL\Projects\Windows\build\Screen.mexw32 T:\projects\OpenGLPsychtoolbox\trunk\Psychtoolbox\PsychBasic\MatlabWindowsFilesR2007a\');
    end

    if what == 1
        % Build WaitSecs.mexw32
        mex -v -outdir T:\projects\OpenGLPsychtoolbox\trunk\PsychSourceGL\Projects\Windows\build\ -output WaitSecs.mexw32 -DPTBMODULE_WaitSecs -I"C:\Programme\Microsoft Visual Studio 8\VC\Include" -ICommon\Base -ICommon\WaitSecs -IWindows\Base Windows\Base\*.c Common\Base\*.c Common\WaitSecs\*.c kernel32.lib user32.lib winmm.lib
        dos('copy T:\projects\OpenGLPsychtoolbox\trunk\PsychSourceGL\Projects\Windows\build\WaitSecs.mexw32 T:\projects\OpenGLPsychtoolbox\trunk\Psychtoolbox\PsychBasic\MatlabWindowsFilesR2007a\');
    end

    if what == 2
        % Build PsychPortAudio.mexw32
        mex -v -outdir T:\projects\OpenGLPsychtoolbox\trunk\PsychSourceGL\Projects\Windows\build\ -output PsychPortAudio.mexw32 -DPTBMODULE_PsychPortAudio -I"C:\Programme\Microsoft Visual Studio 8\VC\Include" -ICommon\Base -ICommon\PsychPortAudio -IWindows\Base Windows\Base\*.c Common\Base\*.c Common\PsychPortAudio\*.c kernel32.lib user32.lib winmm.lib delayimp.lib portaudio_x86.lib LINKFLAGS="$LINKFLAGS /DELAYLOAD:portaudio_x86.dll" 
        dos('copy T:\projects\OpenGLPsychtoolbox\trunk\PsychSourceGL\Projects\Windows\build\PsychPortAudio.mexw32 T:\projects\OpenGLPsychtoolbox\trunk\Psychtoolbox\PsychBasic\MatlabWindowsFilesR2007a\');
    end

    if what == 3
        % Build GetSecs.mexw32
        mex -v -outdir T:\projects\OpenGLPsychtoolbox\trunk\PsychSourceGL\Projects\Windows\build\ -output GetSecs.mexw32 -DPTBMODULE_GetSecs -I"C:\Programme\Microsoft Visual Studio 8\VC\Include" -ICommon\Base -ICommon\GetSecs -IWindows\Base Windows\Base\*.c Common\Base\*.c Common\GetSecs\*.c kernel32.lib user32.lib winmm.lib
        dos('copy T:\projects\OpenGLPsychtoolbox\trunk\PsychSourceGL\Projects\Windows\build\GetSecs.mexw32 T:\projects\OpenGLPsychtoolbox\trunk\Psychtoolbox\PsychBasic\MatlabWindowsFilesR2007a\');
    end

    if what == 4
        % Build IOPort.mexw32
        mex -v -outdir T:\projects\OpenGLPsychtoolbox\trunk\PsychSourceGL\Projects\Windows\build\ -output IOPort.mexw32 -DPTBMODULE_IOPort -I"C:\Programme\Microsoft Visual Studio 8\VC\Include" -ICommon\Base -ICommon\IOPort -IWindows\Base -IWindows\IOPort Windows\Base\*.c Common\Base\*.c Common\IOPort\*.c Windows\IOPort\*.c kernel32.lib user32.lib winmm.lib
        dos('copy T:\projects\OpenGLPsychtoolbox\trunk\PsychSourceGL\Projects\Windows\build\IOPort.mexw32 T:\projects\OpenGLPsychtoolbox\trunk\Psychtoolbox\PsychBasic\MatlabWindowsFilesR2007a\');
    end

    if what == 5
        % Build PsychCV.mexw32
        mex -v -outdir T:\projects\OpenGLPsychtoolbox\trunk\PsychSourceGL\Projects\Windows\build\ -output PsychCV.mexw32 -DPTBMODULE_PsychCV -DTARGET_OS_WIN32 -ID:\install\QuickTimeSDK\CIncludes -I"C:\Programme\Microsoft Visual Studio 8\VC\Include" -I"D:\MicrosoftDirectXSDK\Include" -ICommon\Base -ICommon\PsychCV -IWindows\Base -I..\Cohorts\ARToolkit\include Windows\Base\*.c Common\Base\*.c Common\PsychCV\*.c kernel32.lib user32.lib gdi32.lib advapi32.lib glu32.lib opengl32.lib winmm.lib delayimp.lib libARvideo.lib libARgsub.lib libARgsub_lite.lib libARgsubUtil.lib libARMulti.lib libAR.lib
        dos('copy T:\projects\OpenGLPsychtoolbox\trunk\PsychSourceGL\Projects\Windows\build\PsychCV.mexw32 T:\projects\OpenGLPsychtoolbox\trunk\Psychtoolbox\PsychBasic\MatlabWindowsFilesR2007a\');
    end
else
    % Octave-3 build:
    if what == 0
        % Default: Build Screen.mex
        mexoctave -v --output T:\projects\OpenGLPsychtoolbox\trunk\PsychSourceGL\Projects\Windows\build\Screen.mex -DPTBMODULE_Screen -DPTBVIDEOCAPTURE_ARVIDEO -DPTBVIDEOCAPTURE_QT -DTARGET_OS_WIN32 -ID:\install\QuickTimeSDK\CIncludes -I"C:\Programme\Microsoft Visual Studio 8\VC\Include" -ID:\MicrosoftDirectXSDK\Include -ICommon\Base -ICommon\Screen -IWindows\Base -IWindows\Screen -I..\Cohorts\ARToolkit\include Windows\Screen\*.c Windows\Base\*.c Common\Base\*.c Common\Screen\*.c kernel32.lib user32.lib gdi32.lib advapi32.lib glu32.lib opengl32.lib qtmlClient.lib ddraw.lib winmm.lib delayimp.lib libARvideo.lib
        dos('copy T:\projects\OpenGLPsychtoolbox\trunk\PsychSourceGL\Projects\Windows\build\Screen.mex T:\projects\OpenGLPsychtoolbox\trunk\Psychtoolbox\PsychBasic\Octave3WindowsFiles\');
    end

    if what == 1
        % Build WaitSecs.mex
        mexoctave -v --output T:\projects\OpenGLPsychtoolbox\trunk\PsychSourceGL\Projects\Windows\build\WaitSecs.mex -DPTBMODULE_WaitSecs -I"C:\Programme\Microsoft Visual Studio 8\VC\Include" -ICommon\Base -ICommon\WaitSecs -IWindows\Base Windows\Base\*.c Common\Base\*.c Common\WaitSecs\*.c kernel32.lib user32.lib winmm.lib
        dos('copy T:\projects\OpenGLPsychtoolbox\trunk\PsychSourceGL\Projects\Windows\build\WaitSecs.mex T:\projects\OpenGLPsychtoolbox\trunk\Psychtoolbox\PsychBasic\Octave3WindowsFiles\');
    end

    if what == 2
        % Build PsychPortAudio.mex
        mexoctave -v --output T:\projects\OpenGLPsychtoolbox\trunk\PsychSourceGL\Projects\Windows\build\PsychPortAudio.mex -DPTBMODULE_PsychPortAudio -I"C:\Programme\Microsoft Visual Studio 8\VC\Include" -ICommon\Base -ICommon\PsychPortAudio -IWindows\Base Windows\Base\*.c Common\Base\*.c Common\PsychPortAudio\*.c kernel32.lib user32.lib winmm.lib delayimp.lib portaudio_x86.lib
        dos('copy T:\projects\OpenGLPsychtoolbox\trunk\PsychSourceGL\Projects\Windows\build\PsychPortAudio.mex T:\projects\OpenGLPsychtoolbox\trunk\Psychtoolbox\PsychBasic\Octave3WindowsFiles\');
    end

    if what == 3
        % Build GetSecs.mex
        mexoctave -v --output T:\projects\OpenGLPsychtoolbox\trunk\PsychSourceGL\Projects\Windows\build\GetSecs.mex -DPTBMODULE_GetSecs -I"C:\Programme\Microsoft Visual Studio 8\VC\Include" -ICommon\Base -ICommon\GetSecs -IWindows\Base Windows\Base\*.c Common\Base\*.c Common\GetSecs\*.c kernel32.lib user32.lib winmm.lib
        dos('copy T:\projects\OpenGLPsychtoolbox\trunk\PsychSourceGL\Projects\Windows\build\GetSecs.mex T:\projects\OpenGLPsychtoolbox\trunk\Psychtoolbox\PsychBasic\Octave3WindowsFiles\');
    end

    if what == 4
        % Build IOPort.mex
        mexoctave -v --output T:\projects\OpenGLPsychtoolbox\trunk\PsychSourceGL\Projects\Windows\build\IOPort.mex -DPTBMODULE_IOPort -I"C:\Programme\Microsoft Visual Studio 8\VC\Include" -ICommon\Base -ICommon\IOPort -IWindows\Base -IWindows\IOPort Windows\Base\*.c Common\Base\*.c Common\IOPort\*.c Windows\IOPort\*.c kernel32.lib user32.lib winmm.lib
        dos('copy T:\projects\OpenGLPsychtoolbox\trunk\PsychSourceGL\Projects\Windows\build\IOPort.mex T:\projects\OpenGLPsychtoolbox\trunk\Psychtoolbox\PsychBasic\Octave3WindowsFiles\');
    end

    if what == 5
        % Build PsychCV.mex
        mexoctave -v --output T:\projects\OpenGLPsychtoolbox\trunk\PsychSourceGL\Projects\Windows\build\PsychCV.mex -DPTBMODULE_PsychCV -DTARGET_OS_WIN32 -ID:\install\QuickTimeSDK\CIncludes -I"C:\Programme\Microsoft Visual Studio 8\VC\Include" -ID:\MicrosoftDirectXSDK\Include -ICommon\Base -ICommon\PsychCV -IWindows\Base -I..\Cohorts\ARToolkit\include Windows\Base\*.c Common\Base\*.c Common\PsychCV\*.c kernel32.lib user32.lib gdi32.lib advapi32.lib glu32.lib opengl32.lib winmm.lib delayimp.lib libARvideo.lib libARgsub.lib libARgsub_lite.lib libARgsubUtil.lib libARMulti.lib libAR.lib
        dos('copy T:\projects\OpenGLPsychtoolbox\trunk\PsychSourceGL\Projects\Windows\build\PsychCV.mex T:\projects\OpenGLPsychtoolbox\trunk\Psychtoolbox\PsychBasic\Octave3WindowsFiles\');
    end
	
    if what == 6
        % Build moglcore.mex
		mexoctave -v --output T:\projects\OpenGLPsychtoolbox\trunk\PsychSourceGL\Projects\Windows\build\moglcore.mex -DTARGET_OS_WIN32 -I"C:\Programme\Microsoft Visual Studio 8\VC\Include" -IU:\projects\OpenGLPsychtoolbox\trunk\Psychtoolbox\PsychOpenGL\MOGL\source -DWINDOWS -DGLEW_STATIC windowhacks.c gl_auto.c gl_manual.c mogl_rebinder.c moglcore.c glew.c user32.lib gdi32.lib advapi32.lib glu32.lib opengl32.lib glut32.lib
        dos('copy T:\projects\OpenGLPsychtoolbox\trunk\PsychSourceGL\Projects\Windows\build\moglcore.mex T:\projects\OpenGLPsychtoolbox\trunk\Psychtoolbox\PsychBasic\Octave3WindowsFiles\');
    end
    
    if what == 7
        % Build Eyelink.mex
		mexoctave -v --output T:\projects\OpenGLPsychtoolbox\trunk\PsychSourceGL\Projects\Windows\build\Eyelink.mex -DTARGET_BUILD_WIN32R11 -LD:\SRResearch\EyeLink\libs -ID:\SRResearch\EyeLink\Includes\eyelink -I"C:\Programme\Microsoft Visual Studio 8\VC\Include" -ICommon\Base -ICommon\Eyelink -IWindows\Base Windows\Base\*.c Common\Base\*.c Common\Eyelink\*.c user32.lib gdi32.lib advapi32.lib winmm.lib eyelink_core.lib eyelink_w32_comp.lib eyelink_exptkit20.lib 
        dos('copy T:\projects\OpenGLPsychtoolbox\trunk\PsychSourceGL\Projects\Windows\build\Eyelink.mex T:\projects\OpenGLPsychtoolbox\trunk\Psychtoolbox\PsychBasic\Octave3WindowsFiles\');
    end
    
end

delete('T:\projects\OpenGLPsychtoolbox\trunk\PsychSourceGL\Source\Common\Base\PsychScriptingGlue.c');
return;

% Special mex wrapper for Octave compile on Windows:
function mexoctave(varargin)
	debugme = 0;
	callmex = 1;
	
	if (debugme), fprintf('nargin = %i\n', nargin); end;
	myvararg = cell();
	myvararg(end+1) = '-DPTBOCTAVE3MEX';
    myvararg(end+1) = '-Wno-multichar';
    myvararg(end+1) = '-Wno-unknown-pragmas';
	
	outarg = '';
	quoted = 0;
	emitarg = 0;
	for i=1:nargin
		curarg = char(varargin(i));
		
		if debugme, fprintf('Preparse Arg %i: %s\n', i, curarg); end
		if (length(strfind(curarg, '"')) > 0)
			if ~quoted
				% Start of quoted string:
				quoted = 1;
				outarg = [curarg];
				emitarg = 0;
			else
				% End of quoted string: Emit!
				quoted = 0;
				outarg = [ outarg curarg ];
				emitarg = 1;
			end
		else
			% Not start or end string of a quoted piece:
			if quoted
				% Within a quoted segment!
				outarg = [ outarg curarg ];
				emitarg = 0;
			else
				% Outside a quoted segment:
				% Expansion needed?
				ppos = strfind(curarg, '*.c');
				if length(ppos) > 0
					prefix  = curarg(1:ppos(1)-1);
					allfiles = dir(curarg);
					for j=1:length(allfiles)
						expandedfiles = [ prefix allfiles(j).name ];
						emitarg = 0;
						myvararg(end+1) = expandedfiles;
						if (debugme), fprintf('Emitted Arg %i : %s\n', length(myvararg), char(myvararg(end))); end;
					end
				else
					% Regular chunk: Emit it
					ppos = strfind(curarg, '.lib');
					if ~isempty(ppos)
						curarg = [ '-l' curarg(1:ppos(1)-1) ];
					end
					
					
					outarg = curarg;
					emitarg = 1;
				end
			end
		end
		
		if emitarg
			emitarg = 0;
			myvararg(end+1) = outarg;
			if (debugme), fprintf('Emitted Arg %i : %s\n', length(myvararg), char(myvararg(end))); end;
		end
	end
	
	final = myvararg
	
	if (debugme), outargtype = class(myvararg), end;

	if (callmex)
		mex(myvararg{:});
	end
	
return;
