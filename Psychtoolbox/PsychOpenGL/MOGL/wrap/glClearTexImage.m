function glClearTexImage( texture, level, format, type, data )

% glClearTexImage  Interface to OpenGL function glClearTexImage
%
% usage:  glClearTexImage( texture, level, format, type, data )
%
% C function:  void glClearTexImage(GLuint texture, GLint level, GLenum format, GLenum type, const void* data)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glClearTexImage', texture, level, format, type, data );

return
