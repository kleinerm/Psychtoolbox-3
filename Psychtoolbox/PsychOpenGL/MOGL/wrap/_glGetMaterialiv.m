function params = glGetMaterialiv( face, pname )

% glGetMaterialiv  Interface to OpenGL function glGetMaterialiv
%
% usage:  params = glGetMaterialiv( face, pname )
%
% C function:  void glGetMaterialiv(GLenum face, GLenum pname, GLint* params)

% 05-Mar-2006 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = int32(0);

moglcore( 'glGetMaterialiv', face, pname, params );

return
