function param = glGetNamedFramebufferParameteriv( framebuffer, pname )

% glGetNamedFramebufferParameteriv  Interface to OpenGL function glGetNamedFramebufferParameteriv
%
% usage:  param = glGetNamedFramebufferParameteriv( framebuffer, pname )
%
% C function:  void glGetNamedFramebufferParameteriv(GLuint framebuffer, GLenum pname, GLint* param)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

param = int32(0);

moglcore( 'glGetNamedFramebufferParameteriv', framebuffer, pname, param );

return
