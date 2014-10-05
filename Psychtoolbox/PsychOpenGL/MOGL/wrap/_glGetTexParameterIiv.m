function params = glGetTexParameterIiv( target, pname )

% glGetTexParameterIiv  Interface to OpenGL function glGetTexParameterIiv
%
% usage:  params = glGetTexParameterIiv( target, pname )
%
% C function:  void glGetTexParameterIiv(GLenum target, GLenum pname, GLint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = int32(0);

moglcore( 'glGetTexParameterIiv', target, pname, params );

return
