function glMultiTexCoord3sv( target, v )

% glMultiTexCoord3sv  Interface to OpenGL function glMultiTexCoord3sv
%
% usage:  glMultiTexCoord3sv( target, v )
%
% C function:  void glMultiTexCoord3sv(GLenum target, const GLshort* v)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glMultiTexCoord3sv', target, int16(v) );

return
