function params = glGetLightfv( light, pname )

% glGetLightfv  Interface to OpenGL function glGetLightfv
%
% usage:  params = glGetLightfv( light, pname )
%
% C function:  void glGetLightfv(GLenum light, GLenum pname, GLfloat* params)

% 24-Jan-2006 -- created (generated automatically from header files)

% ---allocate---
% ---protected---
% ---skip---

if nargin~=2,
    error('invalid number of arguments');
end

params = single(NaN(4,1));
moglcore( 'glGetLightfv', light, pname, params );
params = double(params);
params = params(find(~isnan(params))); %#ok<FNDSB>

return
