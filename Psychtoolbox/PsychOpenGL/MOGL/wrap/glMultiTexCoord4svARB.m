function glMultiTexCoord4svARB( target, v )

% glMultiTexCoord4svARB  Interface to OpenGL function glMultiTexCoord4svARB
%
% usage:  glMultiTexCoord4svARB( target, v )
%
% C function:  void glMultiTexCoord4svARB(GLenum target, const GLshort* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glMultiTexCoord4svARB', target, int16(v) );

return
