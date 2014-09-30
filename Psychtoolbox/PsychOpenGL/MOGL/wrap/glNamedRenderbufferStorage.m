function glNamedRenderbufferStorage( renderbuffer, internalformat, width, height )

% glNamedRenderbufferStorage  Interface to OpenGL function glNamedRenderbufferStorage
%
% usage:  glNamedRenderbufferStorage( renderbuffer, internalformat, width, height )
%
% C function:  void glNamedRenderbufferStorage(GLuint renderbuffer, GLenum internalformat, GLsizei width, GLsizei height)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glNamedRenderbufferStorage', renderbuffer, internalformat, width, height );

return
