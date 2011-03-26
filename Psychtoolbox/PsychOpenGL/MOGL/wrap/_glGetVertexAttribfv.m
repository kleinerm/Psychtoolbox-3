function params = glGetVertexAttribfv( index, pname )

% glGetVertexAttribfv  Interface to OpenGL function glGetVertexAttribfv
%
% usage:  params = glGetVertexAttribfv( index, pname )
%
% C function:  void glGetVertexAttribfv(GLuint index, GLenum pname, GLfloat* params)

% 25-Mar-2011 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = single(0);

moglcore( 'glGetVertexAttribfv', index, pname, params );

return
