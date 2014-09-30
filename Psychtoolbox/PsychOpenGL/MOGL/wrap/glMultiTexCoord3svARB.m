function glMultiTexCoord3svARB( target, v )

% glMultiTexCoord3svARB  Interface to OpenGL function glMultiTexCoord3svARB
%
% usage:  glMultiTexCoord3svARB( target, v )
%
% C function:  void glMultiTexCoord3svARB(GLenum target, const GLshort* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glMultiTexCoord3svARB', target, int16(v) );

return
