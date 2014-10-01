function glVariantdvEXT( id, addr )

% glVariantdvEXT  Interface to OpenGL function glVariantdvEXT
%
% usage:  glVariantdvEXT( id, addr )
%
% C function:  void glVariantdvEXT(GLuint id, const GLdouble* addr)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVariantdvEXT', id, double(addr) );

return
