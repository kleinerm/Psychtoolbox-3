function glGetQueryBufferObjecti64v( id, buffer, pname, offset )

% glGetQueryBufferObjecti64v  Interface to OpenGL function glGetQueryBufferObjecti64v
%
% usage:  glGetQueryBufferObjecti64v( id, buffer, pname, offset )
%
% C function:  void glGetQueryBufferObjecti64v(GLuint id, GLuint buffer, GLenum pname, GLintptr offset)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glGetQueryBufferObjecti64v', id, buffer, pname, offset );

return
