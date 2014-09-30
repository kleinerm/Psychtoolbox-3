function glTexRenderbufferNV( target, renderbuffer )

% glTexRenderbufferNV  Interface to OpenGL function glTexRenderbufferNV
%
% usage:  glTexRenderbufferNV( target, renderbuffer )
%
% C function:  void glTexRenderbufferNV(GLenum target, GLuint renderbuffer)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glTexRenderbufferNV', target, renderbuffer );

return
