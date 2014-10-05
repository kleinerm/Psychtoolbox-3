function glTextureParameteriv( texture, pname, param )

% glTextureParameteriv  Interface to OpenGL function glTextureParameteriv
%
% usage:  glTextureParameteriv( texture, pname, param )
%
% C function:  void glTextureParameteriv(GLuint texture, GLenum pname, const GLint* param)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glTextureParameteriv', texture, pname, int32(param) );

return
