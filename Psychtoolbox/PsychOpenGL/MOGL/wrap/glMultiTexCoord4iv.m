function glMultiTexCoord4iv( target, v )

% glMultiTexCoord4iv  Interface to OpenGL function glMultiTexCoord4iv
%
% usage:  glMultiTexCoord4iv( target, v )
%
% C function:  void glMultiTexCoord4iv(GLenum target, const GLint* v)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glMultiTexCoord4iv', target, int32(v) );

return
