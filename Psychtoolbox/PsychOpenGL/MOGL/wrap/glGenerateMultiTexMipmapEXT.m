function glGenerateMultiTexMipmapEXT( texunit, target )

% glGenerateMultiTexMipmapEXT  Interface to OpenGL function glGenerateMultiTexMipmapEXT
%
% usage:  glGenerateMultiTexMipmapEXT( texunit, target )
%
% C function:  void glGenerateMultiTexMipmapEXT(GLenum texunit, GLenum target)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glGenerateMultiTexMipmapEXT', texunit, target );

return
