function glTexSubImage4DSGIS( target, level, xoffset, yoffset, zoffset, woffset, width, height, depth, size4d, format, type, pixels )

% glTexSubImage4DSGIS  Interface to OpenGL function glTexSubImage4DSGIS
%
% usage:  glTexSubImage4DSGIS( target, level, xoffset, yoffset, zoffset, woffset, width, height, depth, size4d, format, type, pixels )
%
% C function:  void glTexSubImage4DSGIS(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint woffset, GLsizei width, GLsizei height, GLsizei depth, GLsizei size4d, GLenum format, GLenum type, const void* pixels)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=13,
    error('invalid number of arguments');
end

moglcore( 'glTexSubImage4DSGIS', target, level, xoffset, yoffset, zoffset, woffset, width, height, depth, size4d, format, type, pixels );

return
