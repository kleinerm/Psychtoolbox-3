function glTextureParameterfvEXT( texture, target, pname, params )

% glTextureParameterfvEXT  Interface to OpenGL function glTextureParameterfvEXT
%
% usage:  glTextureParameterfvEXT( texture, target, pname, params )
%
% C function:  void glTextureParameterfvEXT(GLuint texture, GLenum target, GLenum pname, const GLfloat* params)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glTextureParameterfvEXT', texture, target, pname, single(params) );

return
