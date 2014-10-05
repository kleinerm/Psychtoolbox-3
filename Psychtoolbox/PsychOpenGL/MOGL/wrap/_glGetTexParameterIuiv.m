function params = glGetTexParameterIuiv( target, pname )

% glGetTexParameterIuiv  Interface to OpenGL function glGetTexParameterIuiv
%
% usage:  params = glGetTexParameterIuiv( target, pname )
%
% C function:  void glGetTexParameterIuiv(GLenum target, GLenum pname, GLuint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = uint32(0);

moglcore( 'glGetTexParameterIuiv', target, pname, params );

return
