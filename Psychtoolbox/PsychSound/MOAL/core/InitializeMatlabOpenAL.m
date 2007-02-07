function InitializeMatlabOpenAL(debuglevel, snddevicename, openal_c_style)
% InitializeMatlabOpenAL(debuglevel, snddevicename, openal_c_style)
%
% InitializeMatlabOpenAL -- Initialize the OpenAL for Matlab wrapper 'moal'.
%
% Call this function at the beginning of your experiment script if you intend
% to use low-level OpenAL sound commands in your script as provided by our
% moalcore extension.
%
% This will check if moal is properly installed and upload all required
% OpenAL constants into your Matlab workspace. It will also set up
% Psychtoolbox for interfacing with external OpenAL code.
%
% Options:
% openal_c_style = 0 / 1:
% If you call InitializeMatlabOpenAL or InitializeMatlabOpenAL(0), all
% constants will be loaded in structs in order to avoid cluttering the
% Matlab workspace too much. You'll have to replace all GL_xxx calls by
% GL.xxx calls, e.g., GL_LIGHTING becomes GL.LIGHTING .
% If you call InitializeMatlabOpenGL(1), then all constants will additionally
% be provided in standard C-Style syntax, aka GL_LIGHTING.
%
% debuglevel = 0 to 3: Setting debuglevel == 0 will disable debug-output.
% A level of 1 will cause MOGL to output error messages and abort your
% scripts execution if it detects any OpenGL error. A level of 2 provides
% additional information that may help you to optimize your code. level 3
% means to be very verbose.
%
% The 'OpenGL for Matlab' low level wrapper mogl was developed, implemented
% and contributed to Psychtoolbox under GPL license by
% Prof. Richard F. Murray, University of York, Canada.

% History:
% 02/16/06 mk Written - Based on glmGetconst from Richard F. Murray.

global AL;
global ALC;

% We default to non-C-Style constants if not requested otherwise.
if nargin < 3
    openal_c_style = 0;
end

if isempty(openal_c_style)
   openal_c_style = 0;
end;

if nargin < 2
    snddevicename = [];
end

if nargin < 1
    debuglevel = 1;
end;

if isempty(debuglevel)
    debuglevel = 1;
end;

% Load all GL constants:
evalin('caller','global AL ALC');

% Absolute path to oalconst.mat file: Needed for GNU/Octave:
oalconstpath = [PsychtoolboxRoot 'PsychOpenAL/MOAL/core/oalconst.mat'];

% C-Style compatible load requested?
if openal_c_style > 0
   % Load all constants, also the C-Style ones, e.g., AL_WHATEVER
   evalin('caller',['load ' oalconstpath]);    
else
   % Load only the AL. ALC. versions, e.g., AL.WHATEVER
   % This is less convenient as one needs to replace AL_ by AL. in
   % all scripts, but it doesn't clutter the Matlab workspace...
   evalin('caller',['load ' oalconstpath ' AL ALC']);      
end;

% On Windows, we need to preload moalcore into Matlab while the working
% directory is set to Psychtoolbox/PsychOpenAL/MOAL/core , so the Windows
% dynamic linker can find our own local copy of dll's and link moalcore
% against it.
if IsWin   
   % Windows system: Change working dir to location of our dll's
   olddir = pwd;
   cd([PsychtoolboxRoot 'PsychOpenAL/MOAL/core']);
   % Preload (and thereby link against dlls) moglcore into Matlab. The
   % special command 'PREINIT' forces loading and performs a no-operation.
   moalcore('PREINIT');
   % Now that moalcore is (hopefully) properly loaded, we can revert the working
   % directory to its previous setting:
   cd(olddir);
end;

% Set moalcores debuglevel:
moalcore('DEBUGLEVEL', debuglevel);

% Open and setup the audio device & context:
if isempty(snddevicename)
    % Open default device:
    if debuglevel > 1
        fprintf('InitializeMatlabOpenAL: Trying to initialize default sound device.\n');
    end
    moalcore('almOpen');
else
    % Open specified device:
    if debuglevel > 1
        fprintf('InitializeMatlabOpenAL: Trying to initialize sound device %s.\n', snddevicename);
    end
    moalcore('almOpen', snddevicename);
end

% Clear error state:
alGetError

if debuglevel > 1
    fprintf('OpenAL device is: %s\n', alcGetString(ALC.DEVICE_SPECIFIER));
    fprintf('OpenAL vendor is: %s\n', alGetString(AL.VENDOR));
    fprintf('OpenAL version is: %s\n', alGetString(AL.VERSION));
    fprintf('OpenAL renderer is: %s\n', alGetString(AL.RENDERER));
    fprintf('OpenAL extensions are: %s\n', alGetString(AL.EXTENSIONS));
    fprintf('OpenAL ALC extensions are: %s\n', alcGetString(ALC.EXTENSIONS));
end

% Ready to rock!
return;
