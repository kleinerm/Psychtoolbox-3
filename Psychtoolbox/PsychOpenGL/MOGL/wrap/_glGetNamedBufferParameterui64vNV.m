function params = glGetNamedBufferParameterui64vNV( buffer, pname )

% glGetNamedBufferParameterui64vNV  Interface to OpenGL function glGetNamedBufferParameterui64vNV
%
% usage:  params = glGetNamedBufferParameterui64vNV( buffer, pname )
%
% C function:  void glGetNamedBufferParameterui64vNV(GLuint buffer, GLenum pname, GLuint64EXT* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = uint64(0);

moglcore( 'glGetNamedBufferParameterui64vNV', buffer, pname, params );

return
