function glMultiTexCoord3ivARB( target, v )

% glMultiTexCoord3ivARB  Interface to OpenGL function glMultiTexCoord3ivARB
%
% usage:  glMultiTexCoord3ivARB( target, v )
%
% C function:  void glMultiTexCoord3ivARB(GLenum target, const GLint* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glMultiTexCoord3ivARB', target, int32(v) );

return
