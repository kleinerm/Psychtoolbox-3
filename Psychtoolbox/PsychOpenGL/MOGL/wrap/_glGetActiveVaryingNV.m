function [ length, size, type, name ] = glGetActiveVaryingNV( program, index, bufSize )

% glGetActiveVaryingNV  Interface to OpenGL function glGetActiveVaryingNV
%
% usage:  [ length, size, type, name ] = glGetActiveVaryingNV( program, index, bufSize )
%
% C function:  void glGetActiveVaryingNV(GLuint program, GLuint index, GLsizei bufSize, GLsizei* length, GLsizei* size, GLenum* type, GLchar* name)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=3,
    error('invalid number of arguments');
end

length = int32(0);
size = int32(0);
type = uint32(0);
name = uint8(0);

moglcore( 'glGetActiveVaryingNV', program, index, bufSize, length, size, type, name );

return
