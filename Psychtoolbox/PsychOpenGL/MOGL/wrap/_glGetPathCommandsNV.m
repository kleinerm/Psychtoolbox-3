function commands = glGetPathCommandsNV( path )

% glGetPathCommandsNV  Interface to OpenGL function glGetPathCommandsNV
%
% usage:  commands = glGetPathCommandsNV( path )
%
% C function:  void glGetPathCommandsNV(GLuint path, GLubyte* commands)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=1,
    error('invalid number of arguments');
end

commands = uint8(0);

moglcore( 'glGetPathCommandsNV', path, commands );

return
