function glMultiTexCoord3s( target, s, t, r )

% glMultiTexCoord3s  Interface to OpenGL function glMultiTexCoord3s
%
% usage:  glMultiTexCoord3s( target, s, t, r )
%
% C function:  void glMultiTexCoord3s(GLenum target, GLshort s, GLshort t, GLshort r)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glMultiTexCoord3s', target, s, t, r );

return
