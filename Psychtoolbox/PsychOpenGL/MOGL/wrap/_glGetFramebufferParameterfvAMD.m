function values = glGetFramebufferParameterfvAMD( target, pname, numsamples, pixelindex, size )

% glGetFramebufferParameterfvAMD  Interface to OpenGL function glGetFramebufferParameterfvAMD
%
% usage:  values = glGetFramebufferParameterfvAMD( target, pname, numsamples, pixelindex, size )
%
% C function:  void glGetFramebufferParameterfvAMD(GLenum target, GLenum pname, GLuint numsamples, GLuint pixelindex, GLsizei size, GLfloat* values)

% 08-Aug-2020 -- created (generated automatically from header files)

% ---allocate---

if nargin~=5,
    error('invalid number of arguments');
end

values = single(0);

moglcore( 'glGetFramebufferParameterfvAMD', target, pname, numsamples, pixelindex, size, values );

return
