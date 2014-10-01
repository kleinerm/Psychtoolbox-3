function glTextureParameterfEXT( texture, target, pname, param )

% glTextureParameterfEXT  Interface to OpenGL function glTextureParameterfEXT
%
% usage:  glTextureParameterfEXT( texture, target, pname, param )
%
% C function:  void glTextureParameterfEXT(GLuint texture, GLenum target, GLenum pname, GLfloat param)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glTextureParameterfEXT', texture, target, pname, param );

return
