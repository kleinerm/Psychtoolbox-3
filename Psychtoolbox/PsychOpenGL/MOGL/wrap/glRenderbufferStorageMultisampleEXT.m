function glRenderbufferStorageMultisampleEXT( target, samples, internalformat, width, height )

% glRenderbufferStorageMultisampleEXT  Interface to OpenGL function glRenderbufferStorageMultisampleEXT
%
% usage:  glRenderbufferStorageMultisampleEXT( target, samples, internalformat, width, height )
%
% C function:  void glRenderbufferStorageMultisampleEXT(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glRenderbufferStorageMultisampleEXT', target, samples, internalformat, width, height );

return
