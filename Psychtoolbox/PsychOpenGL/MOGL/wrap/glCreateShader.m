function r = glCreateShader( type )

% glCreateShader  Interface to OpenGL function glCreateShader
%
% usage:  r = glCreateShader( type )
%
% C function:  GLuint glCreateShader(GLenum type)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

r = moglcore( 'glCreateShader', type );

return
