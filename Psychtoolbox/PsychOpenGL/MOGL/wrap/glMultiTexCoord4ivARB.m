function glMultiTexCoord4ivARB( target, v )

% glMultiTexCoord4ivARB  Interface to OpenGL function glMultiTexCoord4ivARB
%
% usage:  glMultiTexCoord4ivARB( target, v )
%
% C function:  void glMultiTexCoord4ivARB(GLenum target, const GLint* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glMultiTexCoord4ivARB', target, int32(v) );

return
