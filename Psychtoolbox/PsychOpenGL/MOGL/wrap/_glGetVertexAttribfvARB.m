function params = glGetVertexAttribfvARB( index, pname )

% glGetVertexAttribfvARB  Interface to OpenGL function glGetVertexAttribfvARB
%
% usage:  params = glGetVertexAttribfvARB( index, pname )
%
% C function:  void glGetVertexAttribfvARB(GLuint index, GLenum pname, GLfloat* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = single(0);

moglcore( 'glGetVertexAttribfvARB', index, pname, params );

return
