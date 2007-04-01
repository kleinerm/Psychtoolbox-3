function params = glGetTexGenfv( coord, pname )

% glGetTexGenfv  Interface to OpenGL function glGetTexGenfv
%
% usage:  params = glGetTexGenfv( coord, pname )
%
% C function:  void glGetTexGenfv(GLenum coord, GLenum pname, GLfloat* params)

% 24-Jan-2006 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = moglsingle(NaN(4,1));
moglcore( 'glGetTexGenfv', coord, pname, params );
params = mogldouble(params);
params = params(find(~isnan(params)));

return
