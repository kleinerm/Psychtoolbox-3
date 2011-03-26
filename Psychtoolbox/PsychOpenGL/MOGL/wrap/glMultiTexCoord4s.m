function glMultiTexCoord4s( target, s, t, r, q )

% glMultiTexCoord4s  Interface to OpenGL function glMultiTexCoord4s
%
% usage:  glMultiTexCoord4s( target, s, t, r, q )
%
% C function:  void glMultiTexCoord4s(GLenum target, GLshort s, GLshort t, GLshort r, GLshort q)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glMultiTexCoord4s', target, s, t, r, q );

return
