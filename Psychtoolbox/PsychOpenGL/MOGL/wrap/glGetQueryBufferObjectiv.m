function glGetQueryBufferObjectiv( id, buffer, pname, offset )

% glGetQueryBufferObjectiv  Interface to OpenGL function glGetQueryBufferObjectiv
%
% usage:  glGetQueryBufferObjectiv( id, buffer, pname, offset )
%
% C function:  void glGetQueryBufferObjectiv(GLuint id, GLuint buffer, GLenum pname, GLintptr offset)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glGetQueryBufferObjectiv', id, buffer, pname, offset );

return
