function glGetQueryBufferObjectuiv( id, buffer, pname, offset )

% glGetQueryBufferObjectuiv  Interface to OpenGL function glGetQueryBufferObjectuiv
%
% usage:  glGetQueryBufferObjectuiv( id, buffer, pname, offset )
%
% C function:  void glGetQueryBufferObjectuiv(GLuint id, GLuint buffer, GLenum pname, GLintptr offset)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glGetQueryBufferObjectuiv', id, buffer, pname, offset );

return
