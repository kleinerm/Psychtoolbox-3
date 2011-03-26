function glMultiTexCoord2sv( target, v )

% glMultiTexCoord2sv  Interface to OpenGL function glMultiTexCoord2sv
%
% usage:  glMultiTexCoord2sv( target, v )
%
% C function:  void glMultiTexCoord2sv(GLenum target, const GLshort* v)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glMultiTexCoord2sv', target, int16(v) );

return
