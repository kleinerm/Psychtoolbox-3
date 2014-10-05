function params = glGetNamedBufferParameteriv( buffer, pname )

% glGetNamedBufferParameteriv  Interface to OpenGL function glGetNamedBufferParameteriv
%
% usage:  params = glGetNamedBufferParameteriv( buffer, pname )
%
% C function:  void glGetNamedBufferParameteriv(GLuint buffer, GLenum pname, GLint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = int32(0);

moglcore( 'glGetNamedBufferParameteriv', buffer, pname, params );

return
