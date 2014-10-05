function params = glGetVertexAttribLui64vNV( index, pname )

% glGetVertexAttribLui64vNV  Interface to OpenGL function glGetVertexAttribLui64vNV
%
% usage:  params = glGetVertexAttribLui64vNV( index, pname )
%
% C function:  void glGetVertexAttribLui64vNV(GLuint index, GLenum pname, GLuint64EXT* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = uint64(0);

moglcore( 'glGetVertexAttribLui64vNV', index, pname, params );

return
