function params = glGetMaterialiv( face, pname )

% glGetMaterialiv  Interface to OpenGL function glGetMaterialiv
%
% usage:  params = glGetMaterialiv( face, pname )
%
% C function:  void glGetMaterialiv(GLenum face, GLenum pname, GLint* params)

% 24-Jan-2006 -- created (generated automatically from header files)

% ---allocate---
% ---protected---

if nargin~=2,
    error('invalid number of arguments');
end

% a hack to find out how many values are returned
f=glGetMaterialfv(face,pname);

params = int32(zeros(size(f)));
moglcore( 'glGetMaterialiv', face, pname, params );

return
% ---skip---
