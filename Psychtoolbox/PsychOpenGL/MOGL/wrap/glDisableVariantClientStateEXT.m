function glDisableVariantClientStateEXT( id )

% glDisableVariantClientStateEXT  Interface to OpenGL function glDisableVariantClientStateEXT
%
% usage:  glDisableVariantClientStateEXT( id )
%
% C function:  void glDisableVariantClientStateEXT(GLuint id)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glDisableVariantClientStateEXT', id );

return
