function glMultiTexCoord1dv( target, v )

% glMultiTexCoord1dv  Interface to OpenGL function glMultiTexCoord1dv
%
% usage:  glMultiTexCoord1dv( target, v )
%
% C function:  void glMultiTexCoord1dv(GLenum target, const GLdouble* v)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glMultiTexCoord1dv', target, double(v) );

return
