function glMultiTexCoord4dv( target, v )

% glMultiTexCoord4dv  Interface to OpenGL function glMultiTexCoord4dv
%
% usage:  glMultiTexCoord4dv( target, v )
%
% C function:  void glMultiTexCoord4dv(GLenum target, const GLdouble* v)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glMultiTexCoord4dv', target, double(v) );

return
