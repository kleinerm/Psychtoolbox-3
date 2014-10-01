function params = glGetVertexAttribLui64vARB( index, pname )

% glGetVertexAttribLui64vARB  Interface to OpenGL function glGetVertexAttribLui64vARB
%
% usage:  params = glGetVertexAttribLui64vARB( index, pname )
%
% C function:  void glGetVertexAttribLui64vARB(GLuint index, GLenum pname, GLuint64EXT* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = uint64(0);

moglcore( 'glGetVertexAttribLui64vARB', index, pname, params );

return
