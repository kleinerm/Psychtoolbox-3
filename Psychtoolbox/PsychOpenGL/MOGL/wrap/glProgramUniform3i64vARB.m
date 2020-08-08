function glProgramUniform3i64vARB( program, location, count, value )

% glProgramUniform3i64vARB  Interface to OpenGL function glProgramUniform3i64vARB
%
% usage:  glProgramUniform3i64vARB( program, location, count, value )
%
% C function:  void glProgramUniform3i64vARB(GLuint program, GLint location, GLsizei count, const GLint64* value)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glProgramUniform3i64vARB', program, location, count, int64(value) );

return
