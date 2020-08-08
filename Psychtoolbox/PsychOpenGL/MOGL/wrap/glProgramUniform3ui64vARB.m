function glProgramUniform3ui64vARB( program, location, count, value )

% glProgramUniform3ui64vARB  Interface to OpenGL function glProgramUniform3ui64vARB
%
% usage:  glProgramUniform3ui64vARB( program, location, count, value )
%
% C function:  void glProgramUniform3ui64vARB(GLuint program, GLint location, GLsizei count, const GLuint64* value)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glProgramUniform3ui64vARB', program, location, count, uint64(value) );

return
