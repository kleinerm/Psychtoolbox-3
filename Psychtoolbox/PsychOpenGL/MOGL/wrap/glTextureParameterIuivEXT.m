function glTextureParameterIuivEXT( texture, target, pname, params )

% glTextureParameterIuivEXT  Interface to OpenGL function glTextureParameterIuivEXT
%
% usage:  glTextureParameterIuivEXT( texture, target, pname, params )
%
% C function:  void glTextureParameterIuivEXT(GLuint texture, GLenum target, GLenum pname, const GLuint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glTextureParameterIuivEXT', texture, target, pname, uint32(params) );

return
