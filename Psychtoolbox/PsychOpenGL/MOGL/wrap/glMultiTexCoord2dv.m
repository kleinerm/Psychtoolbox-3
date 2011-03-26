function glMultiTexCoord2dv( target, v )

% glMultiTexCoord2dv  Interface to OpenGL function glMultiTexCoord2dv
%
% usage:  glMultiTexCoord2dv( target, v )
%
% C function:  void glMultiTexCoord2dv(GLenum target, const GLdouble* v)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glMultiTexCoord2dv', target, double(v) );

return
