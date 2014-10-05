function glTextureParameterivEXT( texture, target, pname, params )

% glTextureParameterivEXT  Interface to OpenGL function glTextureParameterivEXT
%
% usage:  glTextureParameterivEXT( texture, target, pname, params )
%
% C function:  void glTextureParameterivEXT(GLuint texture, GLenum target, GLenum pname, const GLint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glTextureParameterivEXT', texture, target, pname, int32(params) );

return
