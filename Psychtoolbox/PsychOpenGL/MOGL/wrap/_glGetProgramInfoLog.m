function [ length, infoLog ] = glGetProgramInfoLog( program, bufSize )

% glGetProgramInfoLog  Interface to OpenGL function glGetProgramInfoLog
%
% usage:  [ length, infoLog ] = glGetProgramInfoLog( program, bufSize )
%
% C function:  void glGetProgramInfoLog(GLuint program, GLsizei bufSize, GLsizei* length, GLchar* infoLog)

% 05-Mar-2006 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

length = int32(0);
infoLog = uint8(0);

moglcore( 'glGetProgramInfoLog', program, bufSize, length, infoLog );

return
