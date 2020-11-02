function glProgramUniform2i64ARB( program, location, x, y )

% glProgramUniform2i64ARB  Interface to OpenGL function glProgramUniform2i64ARB
%
% usage:  glProgramUniform2i64ARB( program, location, x, y )
%
% C function:  void glProgramUniform2i64ARB(GLuint program, GLint location, GLint64 x, GLint64 y)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glProgramUniform2i64ARB', program, location, int64(x), int64(y) );

return
