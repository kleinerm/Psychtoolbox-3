function glVariantbvEXT( id, addr )

% glVariantbvEXT  Interface to OpenGL function glVariantbvEXT
%
% usage:  glVariantbvEXT( id, addr )
%
% C function:  void glVariantbvEXT(GLuint id, const GLbyte* addr)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVariantbvEXT', id, int8(addr) );

return
