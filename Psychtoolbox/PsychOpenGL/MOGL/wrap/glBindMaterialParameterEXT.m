function r = glBindMaterialParameterEXT( face, value )

% glBindMaterialParameterEXT  Interface to OpenGL function glBindMaterialParameterEXT
%
% usage:  r = glBindMaterialParameterEXT( face, value )
%
% C function:  GLuint glBindMaterialParameterEXT(GLenum face, GLenum value)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

r = moglcore( 'glBindMaterialParameterEXT', face, value );

return
