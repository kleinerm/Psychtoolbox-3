function params = glGetVertexAttribLdvEXT( index, pname )

% glGetVertexAttribLdvEXT  Interface to OpenGL function glGetVertexAttribLdvEXT
%
% usage:  params = glGetVertexAttribLdvEXT( index, pname )
%
% C function:  void glGetVertexAttribLdvEXT(GLuint index, GLenum pname, GLdouble* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = double(0);

moglcore( 'glGetVertexAttribLdvEXT', index, pname, params );

return
