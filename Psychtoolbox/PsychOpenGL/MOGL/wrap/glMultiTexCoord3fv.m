function glMultiTexCoord3fv( target, v )

% glMultiTexCoord3fv  Interface to OpenGL function glMultiTexCoord3fv
%
% usage:  glMultiTexCoord3fv( target, v )
%
% C function:  void glMultiTexCoord3fv(GLenum target, const GLfloat* v)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glMultiTexCoord3fv', target, single(v) );

return
