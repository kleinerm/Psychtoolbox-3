function glMultiTexCoord4sv( target, v )

% glMultiTexCoord4sv  Interface to OpenGL function glMultiTexCoord4sv
%
% usage:  glMultiTexCoord4sv( target, v )
%
% C function:  void glMultiTexCoord4sv(GLenum target, const GLshort* v)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glMultiTexCoord4sv', target, int16(v) );

return
