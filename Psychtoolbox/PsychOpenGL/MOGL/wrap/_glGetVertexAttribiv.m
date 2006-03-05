function params = glGetVertexAttribiv( index, pname )

% glGetVertexAttribiv  Interface to OpenGL function glGetVertexAttribiv
%
% usage:  params = glGetVertexAttribiv( index, pname )
%
% C function:  void glGetVertexAttribiv(GLuint index, GLenum pname, GLint* params)

% 05-Mar-2006 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = int32(0);

moglcore( 'glGetVertexAttribiv', index, pname, params );

return
