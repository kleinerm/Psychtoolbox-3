function glUniform2ui64vARB( location, count, value )

% glUniform2ui64vARB  Interface to OpenGL function glUniform2ui64vARB
%
% usage:  glUniform2ui64vARB( location, count, value )
%
% C function:  void glUniform2ui64vARB(GLint location, GLsizei count, const GLuint64* value)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glUniform2ui64vARB', location, count, uint64(value) );

return
