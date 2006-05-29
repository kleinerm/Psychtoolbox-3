function glGenerateMipmapEXT( target )

% glGenerateMipmapEXT  Interface to OpenGL function glGenerateMipmapEXT
%
% usage:  glGenerateMipmapEXT( target )
%
% C function:  void glGenerateMipmapEXT(GLenum target)

% 30-May-2006 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glGenerateMipmapEXT', target );

return
