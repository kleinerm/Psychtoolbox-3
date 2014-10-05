function r = glBindParameterEXT( value )

% glBindParameterEXT  Interface to OpenGL function glBindParameterEXT
%
% usage:  r = glBindParameterEXT( value )
%
% C function:  GLuint glBindParameterEXT(GLenum value)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

r = moglcore( 'glBindParameterEXT', value );

return
