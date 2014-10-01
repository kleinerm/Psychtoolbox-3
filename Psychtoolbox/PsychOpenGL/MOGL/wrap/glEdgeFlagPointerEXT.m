function glEdgeFlagPointerEXT( stride, count, pointer )

% glEdgeFlagPointerEXT  Interface to OpenGL function glEdgeFlagPointerEXT
%
% usage:  glEdgeFlagPointerEXT( stride, count, pointer )
%
% C function:  void glEdgeFlagPointerEXT(GLsizei stride, GLsizei count, const GLboolean* pointer)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glEdgeFlagPointerEXT', stride, count, uint8(pointer) );

return
