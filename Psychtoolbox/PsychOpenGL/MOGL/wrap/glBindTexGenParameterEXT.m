function r = glBindTexGenParameterEXT( unit, coord, value )

% glBindTexGenParameterEXT  Interface to OpenGL function glBindTexGenParameterEXT
%
% usage:  r = glBindTexGenParameterEXT( unit, coord, value )
%
% C function:  GLuint glBindTexGenParameterEXT(GLenum unit, GLenum coord, GLenum value)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

r = moglcore( 'glBindTexGenParameterEXT', unit, coord, value );

return
