function glMultiTexCoord1iv( target, v )

% glMultiTexCoord1iv  Interface to OpenGL function glMultiTexCoord1iv
%
% usage:  glMultiTexCoord1iv( target, v )
%
% C function:  void glMultiTexCoord1iv(GLenum target, const GLint* v)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glMultiTexCoord1iv', target, int32(v) );

return
