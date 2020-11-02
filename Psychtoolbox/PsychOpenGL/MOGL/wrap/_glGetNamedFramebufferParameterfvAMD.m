function values = glGetNamedFramebufferParameterfvAMD( framebuffer, pname, numsamples, pixelindex, size )

% glGetNamedFramebufferParameterfvAMD  Interface to OpenGL function glGetNamedFramebufferParameterfvAMD
%
% usage:  values = glGetNamedFramebufferParameterfvAMD( framebuffer, pname, numsamples, pixelindex, size )
%
% C function:  void glGetNamedFramebufferParameterfvAMD(GLuint framebuffer, GLenum pname, GLuint numsamples, GLuint pixelindex, GLsizei size, GLfloat* values)

% 08-Aug-2020 -- created (generated automatically from header files)

% ---allocate---

if nargin~=5,
    error('invalid number of arguments');
end

values = single(0);

moglcore( 'glGetNamedFramebufferParameterfvAMD', framebuffer, pname, numsamples, pixelindex, size, values );

return
