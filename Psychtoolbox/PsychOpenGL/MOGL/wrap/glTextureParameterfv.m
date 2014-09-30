function glTextureParameterfv( texture, pname, param )

% glTextureParameterfv  Interface to OpenGL function glTextureParameterfv
%
% usage:  glTextureParameterfv( texture, pname, param )
%
% C function:  void glTextureParameterfv(GLuint texture, GLenum pname, const GLfloat* param)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glTextureParameterfv', texture, pname, single(param) );

return
