function params = glGetVertexAttribdvNV( index, pname )

% glGetVertexAttribdvNV  Interface to OpenGL function glGetVertexAttribdvNV
%
% usage:  params = glGetVertexAttribdvNV( index, pname )
%
% C function:  void glGetVertexAttribdvNV(GLuint index, GLenum pname, GLdouble* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = double(0);

moglcore( 'glGetVertexAttribdvNV', index, pname, params );

return
