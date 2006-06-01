function params = glGetTexLevelParameteriv( target, level, pname )

% glGetTexLevelParameteriv  Interface to OpenGL function glGetTexLevelParameteriv
%
% usage:  params = glGetTexLevelParameteriv( target, level, pname )
%
% C function:  void glGetTexLevelParameteriv(GLenum target, GLint level, GLenum pname, GLint* params)

% 05-Mar-2006 -- created (generated automatically from header files)

% ---allocate---

if nargin~=3,
    error('invalid number of arguments');
end

params = int32(0);

moglcore( 'glGetTexLevelParameteriv', target, level, pname, params );

return
