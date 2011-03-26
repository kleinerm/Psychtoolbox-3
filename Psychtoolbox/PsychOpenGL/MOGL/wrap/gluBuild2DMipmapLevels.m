function r = gluBuild2DMipmapLevels( target, internalFormat, width, height, format, type, level, base, max, data )

% gluBuild2DMipmapLevels  Interface to OpenGL function gluBuild2DMipmapLevels
%
% usage:  r = gluBuild2DMipmapLevels( target, internalFormat, width, height, format, type, level, base, max, data )
%
% C function:  GLint gluBuild2DMipmapLevels(GLenum target, GLint internalFormat, GLsizei width, GLsizei height, GLenum format, GLenum type, GLint level, GLint base, GLint max, const void* data)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=10,
    error('invalid number of arguments');
end

r = moglcore( 'gluBuild2DMipmapLevels', target, internalFormat, width, height, format, type, level, base, max, data );

return
