function params = glGetFragmentMaterialivSGIX( face, pname )

% glGetFragmentMaterialivSGIX  Interface to OpenGL function glGetFragmentMaterialivSGIX
%
% usage:  params = glGetFragmentMaterialivSGIX( face, pname )
%
% C function:  void glGetFragmentMaterialivSGIX(GLenum face, GLenum pname, GLint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = int32(0);

moglcore( 'glGetFragmentMaterialivSGIX', face, pname, params );

return
