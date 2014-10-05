function glBinormalPointerEXT( type, stride, pointer )

% glBinormalPointerEXT  Interface to OpenGL function glBinormalPointerEXT
%
% usage:  glBinormalPointerEXT( type, stride, pointer )
%
% C function:  void glBinormalPointerEXT(GLenum type, GLsizei stride, const void* pointer)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glBinormalPointerEXT', type, stride, pointer );

return
