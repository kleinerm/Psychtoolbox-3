function params = glGetTextureParameterivEXT( texture, target, pname )

% glGetTextureParameterivEXT  Interface to OpenGL function glGetTextureParameterivEXT
%
% usage:  params = glGetTextureParameterivEXT( texture, target, pname )
%
% C function:  void glGetTextureParameterivEXT(GLuint texture, GLenum target, GLenum pname, GLint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=3,
    error('invalid number of arguments');
end

params = int32(0);

moglcore( 'glGetTextureParameterivEXT', texture, target, pname, params );

return
