function glNamedFramebufferParameteri( framebuffer, pname, param )

% glNamedFramebufferParameteri  Interface to OpenGL function glNamedFramebufferParameteri
%
% usage:  glNamedFramebufferParameteri( framebuffer, pname, param )
%
% C function:  void glNamedFramebufferParameteri(GLuint framebuffer, GLenum pname, GLint param)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glNamedFramebufferParameteri', framebuffer, pname, param );

return
