function r = gluBuild1DMipmapLevels( target, internalFormat, width, format, type, level, base, max, data )

% gluBuild1DMipmapLevels  Interface to OpenGL function gluBuild1DMipmapLevels
%
% usage:  r = gluBuild1DMipmapLevels( target, internalFormat, width, format, type, level, base, max, data )
%
% C function:  GLint gluBuild1DMipmapLevels(GLenum target, GLint internalFormat, GLsizei width, GLenum format, GLenum type, GLint level, GLint base, GLint max, const void* data)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=9,
    error('invalid number of arguments');
end

r = moglcore( 'gluBuild1DMipmapLevels', target, internalFormat, width, format, type, level, base, max, data );

return
