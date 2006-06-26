function InitializeMatlabOpenGL(opengl_c_style, debuglevel)
% InitializeMatlabOpenGL(opengl_c_style, debuglevel)
%
% InitializeMatlabOpenGL -- Initialize the OpenGL for Matlab wrapper 'mogl'.
%
% Call this function at the beginning of your experiment script before
% calling *any* Psychtoolbox Screen() command, if you intend to use low-level
% OpenGL drawing commands in your script as provided by Richard Murrays
% moglcore extension.
%
% This will check if mogl is properly installed and upload all required
% OpenGL constants into your Matlab workspace. It will also set up
% Psychtoolbox for interfacing with external OpenGL code.
%
% Options:
% opengl_c_style = 0 / 1:
% If you call InitializeMatlabOpenGL or InitializeMatlabOpenGL(0), all
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

global GL;

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

if isempty(opengl_c_style)
   opengl_c_style = 0;
end;

if nargin < 2
    debuglevel = 1;
end;

if isempty(debuglevel)
    debuglevel = 1;
end;

% Load all GL constants:
evalin('caller','global AGL GL GLU');

% Absolute path to oglconst.mat file: Needed for GNU/Octave:
oglconstpath = [PsychtoolboxRoot 'PsychOpenGL/MOGL/core/oglconst.mat'];

% C-Style compatible load requested?
if opengl_c_style > 0
   % Load all constants, also the C-Style ones, e.g., GL_LIGHTING
   evalin('caller',['load ' oglconstpath]);    
else
   % Load only the GL. GLU. and AGL. versions, e.g., GL.LIGHTING
   % This is less convenient as one needs to replace GL_ by GL. in
   % all scripts, but it doesn't clutter the Matlab workspace...
   evalin('caller',['load ' oglconstpath ' AGL GL GLU']);      
end;

% Assign GL_3D manually - Little hack...
GL.GL_3D = 1537;
GL.GL_2D = 1536;

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

% Set moglcores debuglevel:
moglcore('DEBUGLEVEL', debuglevel);

% Enable support for OpenGL 3D graphics rendering in Psychtoolbox.
Screen('Preference', 'Enable3DGraphics', 1);

% Ready to rock!
return;
