function r = glCreateShader( type )

% glCreateShader  Interface to OpenGL function glCreateShader
%
% usage:  r = glCreateShader( type )
%
% C function:  GLuint glCreateShader(GLenum type)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

r = moglcore( 'glCreateShader', type );

return
