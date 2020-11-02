function glProgramUniform4i64ARB( program, location, x, y, z, w )

% glProgramUniform4i64ARB  Interface to OpenGL function glProgramUniform4i64ARB
%
% usage:  glProgramUniform4i64ARB( program, location, x, y, z, w )
%
% C function:  void glProgramUniform4i64ARB(GLuint program, GLint location, GLint64 x, GLint64 y, GLint64 z, GLint64 w)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=6,
    error('invalid number of arguments');
end

moglcore( 'glProgramUniform4i64ARB', program, location, int64(x), int64(y), int64(z), int64(w) );

return
