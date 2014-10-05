function params = glGetTextureParameterfvEXT( texture, target, pname )

% glGetTextureParameterfvEXT  Interface to OpenGL function glGetTextureParameterfvEXT
%
% usage:  params = glGetTextureParameterfvEXT( texture, target, pname )
%
% C function:  void glGetTextureParameterfvEXT(GLuint texture, GLenum target, GLenum pname, GLfloat* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=3,
    error('invalid number of arguments');
end

params = single(0);

moglcore( 'glGetTextureParameterfvEXT', texture, target, pname, params );

return
