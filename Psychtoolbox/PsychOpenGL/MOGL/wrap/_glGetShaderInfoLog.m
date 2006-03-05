function [ length, infoLog ] = glGetShaderInfoLog( shader, bufSize )

% glGetShaderInfoLog  Interface to OpenGL function glGetShaderInfoLog
%
% usage:  [ length, infoLog ] = glGetShaderInfoLog( shader, bufSize )
%
% C function:  void glGetShaderInfoLog(GLuint shader, GLsizei bufSize, GLsizei* length, GLchar* infoLog)

% 05-Mar-2006 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

length = int32(0);
infoLog = uint8(0);

moglcore( 'glGetShaderInfoLog', shader, bufSize, length, infoLog );

return
