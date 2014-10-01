function glVertexAttribPointerARB( index, size, type, normalized, stride, pointer )

% glVertexAttribPointerARB  Interface to OpenGL function glVertexAttribPointerARB
%
% usage:  glVertexAttribPointerARB( index, size, type, normalized, stride, pointer )
%
% C function:  void glVertexAttribPointerARB(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=6,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttribPointerARB', index, size, type, normalized, stride, pointer );

return
