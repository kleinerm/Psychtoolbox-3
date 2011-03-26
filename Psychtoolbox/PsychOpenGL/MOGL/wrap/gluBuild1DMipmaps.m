function r = gluBuild1DMipmaps( target, internalFormat, width, format, type, data )

% gluBuild1DMipmaps  Interface to OpenGL function gluBuild1DMipmaps
%
% usage:  r = gluBuild1DMipmaps( target, internalFormat, width, format, type, data )
%
% C function:  GLint gluBuild1DMipmaps(GLenum target, GLint internalFormat, GLsizei width, GLenum format, GLenum type, const void* data)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=6,
    error('invalid number of arguments');
end

r = moglcore( 'gluBuild1DMipmaps', target, internalFormat, width, format, type, data );

return
