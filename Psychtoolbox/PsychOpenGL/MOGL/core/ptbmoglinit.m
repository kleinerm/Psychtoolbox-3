function ptbmoglinit(opengl_c_style)
% ptbmoglinit -- Initialize the OpenGL for Matlab wrapper 'mogl'
%
% Call this function at the beginning of your experiment script before
% calling *any other* Psychtoolbox command if you intend to use low-level
% OpenGL drawing commands in your script as provided by Richard Murrays
% moglcore extension.
%
% This will check if mogl is properly installed and upload all required
% OpenGL constants into your Matlab workspace. It will also set up
% Psychtoolbox for interfacing with external OpenGL code.
%
% The 'OpenGL for Matlab' low level wrapper mogl was developed, implemented
% and generously contributed to Psychtoolbox under GPL license by
% Prof. Richard F. Murray, University of York, Canada.

% History:
% 02/16/06 mk Written - Based on glmGetconst from Richard F. Murray.

% Is Psychtoolbox properly installed?
AssertOpenGL

% Is Mogl properly installed?
if ~exist('glmGetConst.m','file'),
    error('Failed to initialize OpenGL for Matlab: Add the ''core'' and ''wrap'' directories of mogl to the MATLAB search path!');
end;

if nargin < 1
   opengl_c_style = 0
end;

% Load all GL constants: We do it the way Richard Murray intended it.
% load constants as fields of a struct, e.g., GL.COLOR_BUFFER_BIT
evalin('caller','global AGL GL GLU');
if opengl_c_style > 0
    if IsOSX
        evalin('caller','load oglconst.mat');    
    end;
    
    if IsLinux
        evalin('caller','load oglconst_linux.mat');    
    end;
end;

% Enable support for OpenGL 3D graphics rendering in Psychtoolbox.
Screen('Preference', 'Enable3DGraphics', 1);

% Ready to rock!
return;
