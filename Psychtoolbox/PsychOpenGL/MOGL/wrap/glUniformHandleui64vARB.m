function glUniformHandleui64vARB( location, count, value )

% glUniformHandleui64vARB  Interface to OpenGL function glUniformHandleui64vARB
%
% usage:  glUniformHandleui64vARB( location, count, value )
%
% C function:  void glUniformHandleui64vARB(GLint location, GLsizei count, const GLuint64* value)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glUniformHandleui64vARB', location, count, uint64(value) );

return
