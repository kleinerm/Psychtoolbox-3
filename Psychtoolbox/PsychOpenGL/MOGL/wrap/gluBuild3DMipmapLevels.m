function r = gluBuild3DMipmapLevels( target, internalFormat, width, height, depth, format, type, level, base, max, data )

% gluBuild3DMipmapLevels  Interface to OpenGL function gluBuild3DMipmapLevels
%
% usage:  r = gluBuild3DMipmapLevels( target, internalFormat, width, height, depth, format, type, level, base, max, data )
%
% C function:  GLint gluBuild3DMipmapLevels(GLenum target, GLint internalFormat, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, GLint level, GLint base, GLint max, const void* data)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=11,
    error('invalid number of arguments');
end

r = moglcore( 'gluBuild3DMipmapLevels', target, internalFormat, width, height, depth, format, type, level, base, max, data );

return
