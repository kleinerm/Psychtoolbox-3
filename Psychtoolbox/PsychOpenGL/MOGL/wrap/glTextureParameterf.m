function glTextureParameterf( texture, pname, param )

% glTextureParameterf  Interface to OpenGL function glTextureParameterf
%
% usage:  glTextureParameterf( texture, pname, param )
%
% C function:  void glTextureParameterf(GLuint texture, GLenum pname, GLfloat param)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glTextureParameterf', texture, pname, param );

return
