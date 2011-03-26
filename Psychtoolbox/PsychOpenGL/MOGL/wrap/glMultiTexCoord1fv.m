function glMultiTexCoord1fv( target, v )

% glMultiTexCoord1fv  Interface to OpenGL function glMultiTexCoord1fv
%
% usage:  glMultiTexCoord1fv( target, v )
%
% C function:  void glMultiTexCoord1fv(GLenum target, const GLfloat* v)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glMultiTexCoord1fv', target, single(v) );

return
