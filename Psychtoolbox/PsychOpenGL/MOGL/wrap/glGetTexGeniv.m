function params = glGetTexGeniv( coord, pname )

% glGetTexGeniv  Interface to OpenGL function glGetTexGeniv
%
% usage:  params = glGetTexGeniv( coord, pname )
%
% C function:  void glGetTexGeniv(GLenum coord, GLenum pname, GLint* params)

% 24-Jan-2006 -- created (generated automatically from header files)

% ---allocate---
% ---protected---

if nargin~=2,
    error('invalid number of arguments');
end

params = int32(repmat(intmax('int32'),[ 32 1 ]));
moglcore( 'glGetTexGeniv', coord, pname, params );
params = params(find(params ~= intmax('int32'))); %#ok<FNDSB>
return
% ---skip---
