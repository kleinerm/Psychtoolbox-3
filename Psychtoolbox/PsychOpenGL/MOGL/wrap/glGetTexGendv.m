function params = glGetTexGendv( coord, pname )

% glGetTexGendv  Interface to OpenGL function glGetTexGendv
%
% usage:  params = glGetTexGendv( coord, pname )
%
% C function:  void glGetTexGendv(GLenum coord, GLenum pname, GLdouble* params)

% 24-Jan-2006 -- created (generated automatically from header files)

% ---allocate---
% ---protected---

if nargin~=2,
    error('invalid number of arguments');
end

params = double(NaN(4,1));
moglcore( 'glGetTexGendv', coord, pname, params );
params = params(find(~isnan(params)));

return
% ---skip---
