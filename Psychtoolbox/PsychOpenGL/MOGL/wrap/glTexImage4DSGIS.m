function glTexImage4DSGIS( target, level, internalformat, width, height, depth, size4d, border, format, type, pixels )

% glTexImage4DSGIS  Interface to OpenGL function glTexImage4DSGIS
%
% usage:  glTexImage4DSGIS( target, level, internalformat, width, height, depth, size4d, border, format, type, pixels )
%
% C function:  void glTexImage4DSGIS(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLsizei size4d, GLint border, GLenum format, GLenum type, const void* pixels)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=11,
    error('invalid number of arguments');
end

moglcore( 'glTexImage4DSGIS', target, level, internalformat, width, height, depth, size4d, border, format, type, pixels );

return
