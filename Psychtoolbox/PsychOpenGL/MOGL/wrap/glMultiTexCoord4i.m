function glMultiTexCoord4i( target, arg2, s, t, r )

% glMultiTexCoord4i  Interface to OpenGL function glMultiTexCoord4i
%
% usage:  glMultiTexCoord4i( target, arg2, s, t, r )
%
% C function:  void glMultiTexCoord4i(GLenum target, GLint, GLint s, GLint t, GLint r)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glMultiTexCoord4i', target, arg2, s, t, r );

return
