function params = glGetVertexAttribfvNV( index, pname )

% glGetVertexAttribfvNV  Interface to OpenGL function glGetVertexAttribfvNV
%
% usage:  params = glGetVertexAttribfvNV( index, pname )
%
% C function:  void glGetVertexAttribfvNV(GLuint index, GLenum pname, GLfloat* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = single(0);

moglcore( 'glGetVertexAttribfvNV', index, pname, params );

return
