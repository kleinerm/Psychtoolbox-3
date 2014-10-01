function glNamedRenderbufferStorageMultisample( renderbuffer, samples, internalformat, width, height )

% glNamedRenderbufferStorageMultisample  Interface to OpenGL function glNamedRenderbufferStorageMultisample
%
% usage:  glNamedRenderbufferStorageMultisample( renderbuffer, samples, internalformat, width, height )
%
% C function:  void glNamedRenderbufferStorageMultisample(GLuint renderbuffer, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glNamedRenderbufferStorageMultisample', renderbuffer, samples, internalformat, width, height );

return
