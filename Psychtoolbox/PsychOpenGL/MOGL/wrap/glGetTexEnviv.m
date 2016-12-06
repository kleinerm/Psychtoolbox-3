function params = glGetTexEnviv( target, pname )

% glGetTexEnviv  Interface to OpenGL function glGetTexEnviv
%
% usage:  params = glGetTexEnviv( target, pname )
%
% C function:  void glGetTexEnviv(GLenum target, GLenum pname, GLint* params)

% 24-Jan-2006 -- created (generated automatically from header files)

% ---allocate---
% ---protected---

if nargin~=2,
    error('invalid number of arguments');
end

params = int32(repmat(intmax('int32'),[ 32 1 ]));
moglcore( 'glGetTexEnviv', target, pname, params );
params = params(find(params ~= intmax('int32'))); %#ok<FNDSB>
return
% ---skip---
