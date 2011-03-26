function glMultiTexCoord3i( target, s, t, r )

% glMultiTexCoord3i  Interface to OpenGL function glMultiTexCoord3i
%
% usage:  glMultiTexCoord3i( target, s, t, r )
%
% C function:  void glMultiTexCoord3i(GLenum target, GLint s, GLint t, GLint r)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glMultiTexCoord3i', target, s, t, r );

return
