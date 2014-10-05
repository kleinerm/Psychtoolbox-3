function params = glGetTextureParameterIuivEXT( texture, target, pname )

% glGetTextureParameterIuivEXT  Interface to OpenGL function glGetTextureParameterIuivEXT
%
% usage:  params = glGetTextureParameterIuivEXT( texture, target, pname )
%
% C function:  void glGetTextureParameterIuivEXT(GLuint texture, GLenum target, GLenum pname, GLuint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=3,
    error('invalid number of arguments');
end

params = uint32(0);

moglcore( 'glGetTextureParameterIuivEXT', texture, target, pname, params );

return
