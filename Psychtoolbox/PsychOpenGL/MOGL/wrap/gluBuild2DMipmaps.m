function r = gluBuild2DMipmaps( target, internalFormat, width, height, format, type, data )

% gluBuild2DMipmaps  Interface to OpenGL function gluBuild2DMipmaps
%
% usage:  r = gluBuild2DMipmaps( target, internalFormat, width, height, format, type, data )
%
% C function:  GLint gluBuild2DMipmaps(GLenum target, GLint internalFormat, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* data)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=7,
    error('invalid number of arguments');
end

r = moglcore( 'gluBuild2DMipmaps', target, internalFormat, width, height, format, type, data );

return
