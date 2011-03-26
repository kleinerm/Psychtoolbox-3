function glMultiTexCoord2s( target, s, t )

% glMultiTexCoord2s  Interface to OpenGL function glMultiTexCoord2s
%
% usage:  glMultiTexCoord2s( target, s, t )
%
% C function:  void glMultiTexCoord2s(GLenum target, GLshort s, GLshort t)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glMultiTexCoord2s', target, s, t );

return
