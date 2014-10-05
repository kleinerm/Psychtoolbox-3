function glMultiTexCoord1svARB( target, v )

% glMultiTexCoord1svARB  Interface to OpenGL function glMultiTexCoord1svARB
%
% usage:  glMultiTexCoord1svARB( target, v )
%
% C function:  void glMultiTexCoord1svARB(GLenum target, const GLshort* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glMultiTexCoord1svARB', target, int16(v) );

return
