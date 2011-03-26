function glMultiTexCoord3iv( target, v )

% glMultiTexCoord3iv  Interface to OpenGL function glMultiTexCoord3iv
%
% usage:  glMultiTexCoord3iv( target, v )
%
% C function:  void glMultiTexCoord3iv(GLenum target, const GLint* v)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glMultiTexCoord3iv', target, int32(v) );

return
