function glTextureParameterIuiv( texture, pname, params )

% glTextureParameterIuiv  Interface to OpenGL function glTextureParameterIuiv
%
% usage:  glTextureParameterIuiv( texture, pname, params )
%
% C function:  void glTextureParameterIuiv(GLuint texture, GLenum pname, const GLuint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glTextureParameterIuiv', texture, pname, uint32(params) );

return
