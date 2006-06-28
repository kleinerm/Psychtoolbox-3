function params = glGetMaterialfv( face, pname )

% glGetMaterialfv  Interface to OpenGL function glGetMaterialfv
%
% usage:  params = glGetMaterialfv( face, pname )
%
% C function:  void glGetMaterialfv(GLenum face, GLenum pname, GLfloat* params)

% 24-Jan-2006 -- created (generated automatically from header files)

% ---allocate---
% ---protected---

if nargin~=2,
    error('invalid number of arguments');
end

params = moglsingle(NaN(4,1));
moglcore( 'glGetMaterialfv', face, pname, params );
params = params(find(~isnan(params)));

return
