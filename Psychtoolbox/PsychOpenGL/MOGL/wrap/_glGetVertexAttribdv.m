function params = glGetVertexAttribdv( index, pname )

% glGetVertexAttribdv  Interface to OpenGL function glGetVertexAttribdv
%
% usage:  params = glGetVertexAttribdv( index, pname )
%
% C function:  void glGetVertexAttribdv(GLuint index, GLenum pname, GLdouble* params)

% 05-Mar-2006 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = double(0);

moglcore( 'glGetVertexAttribdv', index, pname, params );

return
