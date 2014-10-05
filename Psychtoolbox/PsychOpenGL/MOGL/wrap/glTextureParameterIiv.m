function glTextureParameterIiv( texture, pname, params )

% glTextureParameterIiv  Interface to OpenGL function glTextureParameterIiv
%
% usage:  glTextureParameterIiv( texture, pname, params )
%
% C function:  void glTextureParameterIiv(GLuint texture, GLenum pname, const GLint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glTextureParameterIiv', texture, pname, int32(params) );

return
