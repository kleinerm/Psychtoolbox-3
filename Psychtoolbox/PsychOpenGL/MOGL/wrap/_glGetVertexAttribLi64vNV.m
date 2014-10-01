function params = glGetVertexAttribLi64vNV( index, pname )

% glGetVertexAttribLi64vNV  Interface to OpenGL function glGetVertexAttribLi64vNV
%
% usage:  params = glGetVertexAttribLi64vNV( index, pname )
%
% C function:  void glGetVertexAttribLi64vNV(GLuint index, GLenum pname, GLint64EXT* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = int64(0);

moglcore( 'glGetVertexAttribLi64vNV', index, pname, params );

return
