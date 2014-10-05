function glTextureNormalEXT( mode )

% glTextureNormalEXT  Interface to OpenGL function glTextureNormalEXT
%
% usage:  glTextureNormalEXT( mode )
%
% C function:  void glTextureNormalEXT(GLenum mode)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glTextureNormalEXT', mode );

return
