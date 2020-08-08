function glProgramUniform4ui64ARB( program, location, x, y, z, w )

% glProgramUniform4ui64ARB  Interface to OpenGL function glProgramUniform4ui64ARB
%
% usage:  glProgramUniform4ui64ARB( program, location, x, y, z, w )
%
% C function:  void glProgramUniform4ui64ARB(GLuint program, GLint location, GLuint64 x, GLuint64 y, GLuint64 z, GLuint64 w)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=6,
    error('invalid number of arguments');
end

moglcore( 'glProgramUniform4ui64ARB', program, location, uint64(x), uint64(y), uint64(z), uint64(w) );

return
