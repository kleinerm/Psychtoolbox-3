function glProgramUniform1i64ARB( program, location, x )

% glProgramUniform1i64ARB  Interface to OpenGL function glProgramUniform1i64ARB
%
% usage:  glProgramUniform1i64ARB( program, location, x )
%
% C function:  void glProgramUniform1i64ARB(GLuint program, GLint location, GLint64 x)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glProgramUniform1i64ARB', program, location, int64(x) );

return
