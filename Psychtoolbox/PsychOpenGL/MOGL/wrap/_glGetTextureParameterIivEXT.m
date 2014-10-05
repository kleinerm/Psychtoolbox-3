function params = glGetTextureParameterIivEXT( texture, target, pname )

% glGetTextureParameterIivEXT  Interface to OpenGL function glGetTextureParameterIivEXT
%
% usage:  params = glGetTextureParameterIivEXT( texture, target, pname )
%
% C function:  void glGetTextureParameterIivEXT(GLuint texture, GLenum target, GLenum pname, GLint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=3,
    error('invalid number of arguments');
end

params = int32(0);

moglcore( 'glGetTextureParameterIivEXT', texture, target, pname, params );

return
