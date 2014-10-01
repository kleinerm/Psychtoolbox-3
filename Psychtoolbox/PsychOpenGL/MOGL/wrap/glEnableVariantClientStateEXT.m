function glEnableVariantClientStateEXT( id )

% glEnableVariantClientStateEXT  Interface to OpenGL function glEnableVariantClientStateEXT
%
% usage:  glEnableVariantClientStateEXT( id )
%
% C function:  void glEnableVariantClientStateEXT(GLuint id)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glEnableVariantClientStateEXT', id );

return
