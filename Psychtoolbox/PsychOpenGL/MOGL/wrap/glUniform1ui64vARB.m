function glUniform1ui64vARB( location, count, value )

% glUniform1ui64vARB  Interface to OpenGL function glUniform1ui64vARB
%
% usage:  glUniform1ui64vARB( location, count, value )
%
% C function:  void glUniform1ui64vARB(GLint location, GLsizei count, const GLuint64* value)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glUniform1ui64vARB', location, count, uint64(value) );

return
