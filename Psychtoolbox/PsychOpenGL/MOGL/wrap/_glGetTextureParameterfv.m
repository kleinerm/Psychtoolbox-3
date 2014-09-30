function params = glGetTextureParameterfv( texture, pname )

% glGetTextureParameterfv  Interface to OpenGL function glGetTextureParameterfv
%
% usage:  params = glGetTextureParameterfv( texture, pname )
%
% C function:  void glGetTextureParameterfv(GLuint texture, GLenum pname, GLfloat* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = single(0);

moglcore( 'glGetTextureParameterfv', texture, pname, params );

return
