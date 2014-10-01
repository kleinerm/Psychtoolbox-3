function glMultiTexCoord1ivARB( target, v )

% glMultiTexCoord1ivARB  Interface to OpenGL function glMultiTexCoord1ivARB
%
% usage:  glMultiTexCoord1ivARB( target, v )
%
% C function:  void glMultiTexCoord1ivARB(GLenum target, const GLint* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glMultiTexCoord1ivARB', target, int32(v) );

return
