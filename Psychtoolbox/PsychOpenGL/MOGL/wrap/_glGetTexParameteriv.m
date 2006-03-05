function params = glGetTexParameteriv( target, pname )

% glGetTexParameteriv  Interface to OpenGL function glGetTexParameteriv
%
% usage:  params = glGetTexParameteriv( target, pname )
%
% C function:  void glGetTexParameteriv(GLenum target, GLenum pname, GLint* params)

% 05-Mar-2006 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = int32(0);

moglcore( 'glGetTexParameteriv', target, pname, params );

return
