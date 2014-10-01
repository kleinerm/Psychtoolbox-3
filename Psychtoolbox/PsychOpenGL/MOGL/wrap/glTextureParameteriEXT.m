function glTextureParameteriEXT( texture, target, pname, param )

% glTextureParameteriEXT  Interface to OpenGL function glTextureParameteriEXT
%
% usage:  glTextureParameteriEXT( texture, target, pname, param )
%
% C function:  void glTextureParameteriEXT(GLuint texture, GLenum target, GLenum pname, GLint param)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glTextureParameteriEXT', texture, target, pname, param );

return
