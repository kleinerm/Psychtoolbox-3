function params = glGetMaterialfv( face, pname )

% glGetMaterialfv  Interface to OpenGL function glGetMaterialfv
%
% usage:  params = glGetMaterialfv( face, pname )
%
% C function:  void glGetMaterialfv(GLenum face, GLenum pname, GLfloat* params)

% 05-Mar-2006 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = single(0);

moglcore( 'glGetMaterialfv', face, pname, params );

return
