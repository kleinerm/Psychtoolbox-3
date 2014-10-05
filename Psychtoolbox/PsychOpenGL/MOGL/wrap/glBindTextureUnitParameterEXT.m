function r = glBindTextureUnitParameterEXT( unit, value )

% glBindTextureUnitParameterEXT  Interface to OpenGL function glBindTextureUnitParameterEXT
%
% usage:  r = glBindTextureUnitParameterEXT( unit, value )
%
% C function:  GLuint glBindTextureUnitParameterEXT(GLenum unit, GLenum value)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

r = moglcore( 'glBindTextureUnitParameterEXT', unit, value );

return
