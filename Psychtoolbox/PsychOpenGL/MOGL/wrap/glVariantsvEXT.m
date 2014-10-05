function glVariantsvEXT( id, addr )

% glVariantsvEXT  Interface to OpenGL function glVariantsvEXT
%
% usage:  glVariantsvEXT( id, addr )
%
% C function:  void glVariantsvEXT(GLuint id, const GLshort* addr)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVariantsvEXT', id, int16(addr) );

return
