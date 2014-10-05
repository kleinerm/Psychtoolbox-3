function glTextureLightEXT( pname )

% glTextureLightEXT  Interface to OpenGL function glTextureLightEXT
%
% usage:  glTextureLightEXT( pname )
%
% C function:  void glTextureLightEXT(GLenum pname)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glTextureLightEXT', pname );

return
