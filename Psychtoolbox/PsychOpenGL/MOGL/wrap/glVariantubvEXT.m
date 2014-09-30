function glVariantubvEXT( id, addr )

% glVariantubvEXT  Interface to OpenGL function glVariantubvEXT
%
% usage:  glVariantubvEXT( id, addr )
%
% C function:  void glVariantubvEXT(GLuint id, const GLubyte* addr)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVariantubvEXT', id, uint8(addr) );

return
