function params = glGetFragmentMaterialfvSGIX( face, pname )

% glGetFragmentMaterialfvSGIX  Interface to OpenGL function glGetFragmentMaterialfvSGIX
%
% usage:  params = glGetFragmentMaterialfvSGIX( face, pname )
%
% C function:  void glGetFragmentMaterialfvSGIX(GLenum face, GLenum pname, GLfloat* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = single(0);

moglcore( 'glGetFragmentMaterialfvSGIX', face, pname, params );

return
