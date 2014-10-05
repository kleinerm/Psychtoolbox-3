function params = glGetNamedBufferParameteri64v( buffer, pname )

% glGetNamedBufferParameteri64v  Interface to OpenGL function glGetNamedBufferParameteri64v
%
% usage:  params = glGetNamedBufferParameteri64v( buffer, pname )
%
% C function:  void glGetNamedBufferParameteri64v(GLuint buffer, GLenum pname, GLint64* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = int64(0);

moglcore( 'glGetNamedBufferParameteri64v', buffer, pname, params );

return
