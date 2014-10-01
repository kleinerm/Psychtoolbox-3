function glTextureParameterIivEXT( texture, target, pname, params )

% glTextureParameterIivEXT  Interface to OpenGL function glTextureParameterIivEXT
%
% usage:  glTextureParameterIivEXT( texture, target, pname, params )
%
% C function:  void glTextureParameterIivEXT(GLuint texture, GLenum target, GLenum pname, const GLint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glTextureParameterIivEXT', texture, target, pname, int32(params) );

return
