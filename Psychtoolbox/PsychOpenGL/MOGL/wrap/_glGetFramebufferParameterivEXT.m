function params = glGetFramebufferParameterivEXT( framebuffer, pname )

% glGetFramebufferParameterivEXT  Interface to OpenGL function glGetFramebufferParameterivEXT
%
% usage:  params = glGetFramebufferParameterivEXT( framebuffer, pname )
%
% C function:  void glGetFramebufferParameterivEXT(GLuint framebuffer, GLenum pname, GLint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = int32(0);

moglcore( 'glGetFramebufferParameterivEXT', framebuffer, pname, params );

return
