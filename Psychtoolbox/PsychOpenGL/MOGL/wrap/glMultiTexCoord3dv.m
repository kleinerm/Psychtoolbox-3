function glMultiTexCoord3dv( target, v )

% glMultiTexCoord3dv  Interface to OpenGL function glMultiTexCoord3dv
%
% usage:  glMultiTexCoord3dv( target, v )
%
% C function:  void glMultiTexCoord3dv(GLenum target, const GLdouble* v)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glMultiTexCoord3dv', target, double(v) );

return
