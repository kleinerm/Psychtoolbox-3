function params = glGetVertexAttribivNV( index, pname )

% glGetVertexAttribivNV  Interface to OpenGL function glGetVertexAttribivNV
%
% usage:  params = glGetVertexAttribivNV( index, pname )
%
% C function:  void glGetVertexAttribivNV(GLuint index, GLenum pname, GLint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = int32(0);

moglcore( 'glGetVertexAttribivNV', index, pname, params );

return
