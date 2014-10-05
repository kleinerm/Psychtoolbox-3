function params = glGetVertexAttribivARB( index, pname )

% glGetVertexAttribivARB  Interface to OpenGL function glGetVertexAttribivARB
%
% usage:  params = glGetVertexAttribivARB( index, pname )
%
% C function:  void glGetVertexAttribivARB(GLuint index, GLenum pname, GLint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = int32(0);

moglcore( 'glGetVertexAttribivARB', index, pname, params );

return
