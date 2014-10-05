function glNamedRenderbufferStorageMultisampleEXT( renderbuffer, samples, internalformat, width, height )

% glNamedRenderbufferStorageMultisampleEXT  Interface to OpenGL function glNamedRenderbufferStorageMultisampleEXT
%
% usage:  glNamedRenderbufferStorageMultisampleEXT( renderbuffer, samples, internalformat, width, height )
%
% C function:  void glNamedRenderbufferStorageMultisampleEXT(GLuint renderbuffer, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glNamedRenderbufferStorageMultisampleEXT', renderbuffer, samples, internalformat, width, height );

return
