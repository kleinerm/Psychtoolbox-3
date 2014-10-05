function glNamedRenderbufferStorageEXT( renderbuffer, internalformat, width, height )

% glNamedRenderbufferStorageEXT  Interface to OpenGL function glNamedRenderbufferStorageEXT
%
% usage:  glNamedRenderbufferStorageEXT( renderbuffer, internalformat, width, height )
%
% C function:  void glNamedRenderbufferStorageEXT(GLuint renderbuffer, GLenum internalformat, GLsizei width, GLsizei height)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glNamedRenderbufferStorageEXT', renderbuffer, internalformat, width, height );

return
