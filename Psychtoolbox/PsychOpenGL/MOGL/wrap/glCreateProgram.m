function r = glCreateProgram

% glCreateProgram  Interface to OpenGL function glCreateProgram
%
% usage:  r = glCreateProgram
%
% C function:  GLuint glCreateProgram(void)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=0,
    error('invalid number of arguments');
end

r = moglcore( 'glCreateProgram' );

return
