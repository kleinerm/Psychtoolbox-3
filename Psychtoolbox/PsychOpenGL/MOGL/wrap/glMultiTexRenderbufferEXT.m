function glMultiTexRenderbufferEXT( texunit, target, renderbuffer )

% glMultiTexRenderbufferEXT  Interface to OpenGL function glMultiTexRenderbufferEXT
%
% usage:  glMultiTexRenderbufferEXT( texunit, target, renderbuffer )
%
% C function:  void glMultiTexRenderbufferEXT(GLenum texunit, GLenum target, GLuint renderbuffer)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glMultiTexRenderbufferEXT', texunit, target, renderbuffer );

return
