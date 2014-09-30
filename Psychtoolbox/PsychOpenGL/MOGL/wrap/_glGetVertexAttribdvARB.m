function params = glGetVertexAttribdvARB( index, pname )

% glGetVertexAttribdvARB  Interface to OpenGL function glGetVertexAttribdvARB
%
% usage:  params = glGetVertexAttribdvARB( index, pname )
%
% C function:  void glGetVertexAttribdvARB(GLuint index, GLenum pname, GLdouble* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = double(0);

moglcore( 'glGetVertexAttribdvARB', index, pname, params );

return
