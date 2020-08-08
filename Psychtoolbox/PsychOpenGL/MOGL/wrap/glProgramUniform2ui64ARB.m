function glProgramUniform2ui64ARB( program, location, x, y )

% glProgramUniform2ui64ARB  Interface to OpenGL function glProgramUniform2ui64ARB
%
% usage:  glProgramUniform2ui64ARB( program, location, x, y )
%
% C function:  void glProgramUniform2ui64ARB(GLuint program, GLint location, GLuint64 x, GLuint64 y)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glProgramUniform2ui64ARB', program, location, uint64(x), uint64(y) );

return
