function params = glGetNamedBufferParameterivEXT( buffer, pname )

% glGetNamedBufferParameterivEXT  Interface to OpenGL function glGetNamedBufferParameterivEXT
%
% usage:  params = glGetNamedBufferParameterivEXT( buffer, pname )
%
% C function:  void glGetNamedBufferParameterivEXT(GLuint buffer, GLenum pname, GLint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = int32(0);

moglcore( 'glGetNamedBufferParameterivEXT', buffer, pname, params );

return
