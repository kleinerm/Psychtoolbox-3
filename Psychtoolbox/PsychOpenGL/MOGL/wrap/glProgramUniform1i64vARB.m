function glProgramUniform1i64vARB( program, location, count, value )

% glProgramUniform1i64vARB  Interface to OpenGL function glProgramUniform1i64vARB
%
% usage:  glProgramUniform1i64vARB( program, location, count, value )
%
% C function:  void glProgramUniform1i64vARB(GLuint program, GLint location, GLsizei count, const GLint64* value)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glProgramUniform1i64vARB', program, location, count, int64(value) );

return
