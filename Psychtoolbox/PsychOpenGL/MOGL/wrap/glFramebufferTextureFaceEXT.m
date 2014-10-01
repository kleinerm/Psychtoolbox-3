function glFramebufferTextureFaceEXT( target, attachment, texture, level, face )

% glFramebufferTextureFaceEXT  Interface to OpenGL function glFramebufferTextureFaceEXT
%
% usage:  glFramebufferTextureFaceEXT( target, attachment, texture, level, face )
%
% C function:  void glFramebufferTextureFaceEXT(GLenum target, GLenum attachment, GLuint texture, GLint level, GLenum face)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glFramebufferTextureFaceEXT', target, attachment, texture, level, face );

return
