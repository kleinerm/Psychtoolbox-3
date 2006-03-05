function params = glGetBufferParameteriv( target, pname )

% glGetBufferParameteriv  Interface to OpenGL function glGetBufferParameteriv
%
% usage:  params = glGetBufferParameteriv( target, pname )
%
% C function:  void glGetBufferParameteriv(GLenum target, GLenum pname, GLint* params)

% 05-Mar-2006 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = int32(0);

moglcore( 'glGetBufferParameteriv', target, pname, params );

return
