function glMultiTexCoord2svARB( target, v )

% glMultiTexCoord2svARB  Interface to OpenGL function glMultiTexCoord2svARB
%
% usage:  glMultiTexCoord2svARB( target, v )
%
% C function:  void glMultiTexCoord2svARB(GLenum target, const GLshort* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glMultiTexCoord2svARB', target, int16(v) );

return
