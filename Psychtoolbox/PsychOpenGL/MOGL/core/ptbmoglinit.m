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
% If you call ptbmoglinit or ptbmoglinit(0), all constants will be loaded
% in structs in order to avoid cluttering the Matlab workspace too much.
% You'll have to replace all GL_xxx calls by GL.xxx calls, e.g., GL_LIGHTING
% becomes GL.LIGHTING
% If you call ptbmoglinit(1), then all constants will also be provided in
% standard C-Style, aka GL_LIGHTING.
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

% We default to non-C-Style constants if not requested otherwise.
if nargin < 1
   opengl_c_style = 0;
end;

% Load all GL constants:
evalin('caller','global AGL GL GLU');

% C-Style compatible load requested?
if opengl_c_style > 0
   % Load all constants, also the C-Style ones, e.g., GL_LIGHTING
   evalin('caller','load oglconst.mat');    
else
   % Load only the GL. GLU. and AGL. versions, e.g., GL.LIGHTING
   % This is less convenient as one needs to replace GL_ by GL. in
   % all scripts, but it doesn't clutter the Matlab workspace...
   evalin('caller','load oglconst.mat AGL GL GLU');      
end;

% On Windows, we need to preload moglcore into Matlab while the working
% directory is set to Psychtoolbox/PsychOpenGL/MOGL/core , so the Windows
% dynamic linker can find our own local copy of glut32.dll and link moglcore
% against it.
if IsWin   
   % Windows system: Change working dir to location of our glut32.dll
   olddir = pwd;
   cd([PsychtoolboxRoot 'PsychOpenGL/MOGL/core']);
   % Preload (and thereby link against glut32.dll) moglcore into Matlab. The
   % special command 'PREINIT' forces loading and performs a no-operation.
   moglcore('PREINIT');
   % Now that moglcore is (hopefully) properly loaded, we can revert the working
   % directory to its previous setting:
   cd(olddir);
end;

% Enable support for OpenGL 3D graphics rendering in Psychtoolbox.
Screen('Preference', 'Enable3DGraphics', 1);

% Ready to rock!
return;
