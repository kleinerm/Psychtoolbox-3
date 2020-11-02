function [ length, params ] = glGetProgramResourcefvNV( program, programInterface, index, propCount, props, bufSize )

% glGetProgramResourcefvNV  Interface to OpenGL function glGetProgramResourcefvNV
%
% usage:  [ length, params ] = glGetProgramResourcefvNV( program, programInterface, index, propCount, props, bufSize )
%
% C function:  void glGetProgramResourcefvNV(GLuint program, GLenum programInterface, GLuint index, GLsizei propCount, const GLenum* props, GLsizei bufSize, GLsizei* length, GLfloat* params)

% 08-Aug-2020 -- created (generated automatically from header files)

% ---allocate---

if nargin~=6,
    error('invalid number of arguments');
end

length = int32(0);
params = single(0);

moglcore( 'glGetProgramResourcefvNV', program, programInterface, index, propCount, uint32(props), bufSize, length, params );

return
