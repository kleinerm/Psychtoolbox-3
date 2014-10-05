function glVertexWeightPointerEXT( size, type, stride, pointer )

% glVertexWeightPointerEXT  Interface to OpenGL function glVertexWeightPointerEXT
%
% usage:  glVertexWeightPointerEXT( size, type, stride, pointer )
%
% C function:  void glVertexWeightPointerEXT(GLint size, GLenum type, GLsizei stride, const void* pointer)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glVertexWeightPointerEXT', size, type, stride, pointer );

return
