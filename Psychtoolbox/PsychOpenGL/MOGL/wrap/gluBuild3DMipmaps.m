function r = gluBuild3DMipmaps( target, internalFormat, width, height, depth, format, type, data )

% gluBuild3DMipmaps  Interface to OpenGL function gluBuild3DMipmaps
%
% usage:  r = gluBuild3DMipmaps( target, internalFormat, width, height, depth, format, type, data )
%
% C function:  GLint gluBuild3DMipmaps(GLenum target, GLint internalFormat, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void* data)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=8,
    error('invalid number of arguments');
end

r = moglcore( 'gluBuild3DMipmaps', target, internalFormat, width, height, depth, format, type, data );

return
