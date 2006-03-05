function [ length, source ] = glGetShaderSource( shader, bufSize )

% glGetShaderSource  Interface to OpenGL function glGetShaderSource
%
% usage:  [ length, source ] = glGetShaderSource( shader, bufSize )
%
% C function:  void glGetShaderSource(GLuint shader, GLsizei bufSize, GLsizei* length, GLchar* source)

% 05-Mar-2006 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

length = int32(0);
source = uint8(0);

moglcore( 'glGetShaderSource', shader, bufSize, length, source );

return
