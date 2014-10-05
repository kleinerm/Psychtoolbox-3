function glMultiTexCoord2ivARB( target, v )

% glMultiTexCoord2ivARB  Interface to OpenGL function glMultiTexCoord2ivARB
%
% usage:  glMultiTexCoord2ivARB( target, v )
%
% C function:  void glMultiTexCoord2ivARB(GLenum target, const GLint* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glMultiTexCoord2ivARB', target, int32(v) );

return
