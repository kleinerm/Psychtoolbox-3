function glMultiTexCoord2iv( target, v )

% glMultiTexCoord2iv  Interface to OpenGL function glMultiTexCoord2iv
%
% usage:  glMultiTexCoord2iv( target, v )
%
% C function:  void glMultiTexCoord2iv(GLenum target, const GLint* v)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glMultiTexCoord2iv', target, int32(v) );

return
