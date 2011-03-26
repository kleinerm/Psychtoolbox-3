function params = glGetTexParameterfv( target, pname )

% glGetTexParameterfv  Interface to OpenGL function glGetTexParameterfv
%
% usage:  params = glGetTexParameterfv( target, pname )
%
% C function:  void glGetTexParameterfv(GLenum target, GLenum pname, GLfloat* params)

% 24-Jan-2006 -- created (generated automatically from header files)

% ---allocate---
% ---protected---
% ---skip---

if nargin~=2,
    error('invalid number of arguments');
end

params = single(NaN(4,1));
moglcore( 'glGetTexParameterfv', target, pname, params );
params = double(params);
params = params(find(~isnan(params))); %#ok<FNDSB>

return
