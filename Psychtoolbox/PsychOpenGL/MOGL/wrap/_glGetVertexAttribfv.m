function params = glGetVertexAttribfv( index, pname )

% glGetVertexAttribfv  Interface to OpenGL function glGetVertexAttribfv
%
% usage:  params = glGetVertexAttribfv( index, pname )
%
% C function:  void glGetVertexAttribfv(GLuint index, GLenum pname, GLfloat* params)

% 05-Mar-2006 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = moglsingle(0);

moglcore( 'glGetVertexAttribfv', index, pname, params );

return
