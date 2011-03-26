function glMultiTexCoord1s( target, s )

% glMultiTexCoord1s  Interface to OpenGL function glMultiTexCoord1s
%
% usage:  glMultiTexCoord1s( target, s )
%
% C function:  void glMultiTexCoord1s(GLenum target, GLshort s)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glMultiTexCoord1s', target, s );

return
