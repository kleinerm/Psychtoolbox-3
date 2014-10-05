function r = glBindLightParameterEXT( light, value )

% glBindLightParameterEXT  Interface to OpenGL function glBindLightParameterEXT
%
% usage:  r = glBindLightParameterEXT( light, value )
%
% C function:  GLuint glBindLightParameterEXT(GLenum light, GLenum value)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

r = moglcore( 'glBindLightParameterEXT', light, value );

return
