function glGetQueryBufferObjectui64v( id, buffer, pname, offset )

% glGetQueryBufferObjectui64v  Interface to OpenGL function glGetQueryBufferObjectui64v
%
% usage:  glGetQueryBufferObjectui64v( id, buffer, pname, offset )
%
% C function:  void glGetQueryBufferObjectui64v(GLuint id, GLuint buffer, GLenum pname, GLintptr offset)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glGetQueryBufferObjectui64v', id, buffer, pname, offset );

return
