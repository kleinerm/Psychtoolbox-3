function glProgramUniform3i64ARB( program, location, x, y, z )

% glProgramUniform3i64ARB  Interface to OpenGL function glProgramUniform3i64ARB
%
% usage:  glProgramUniform3i64ARB( program, location, x, y, z )
%
% C function:  void glProgramUniform3i64ARB(GLuint program, GLint location, GLint64 x, GLint64 y, GLint64 z)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glProgramUniform3i64ARB', program, location, int64(x), int64(y), int64(z) );

return
