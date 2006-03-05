function [ length, size, type, name ] = glGetActiveAttrib( program, index, bufSize )

% glGetActiveAttrib  Interface to OpenGL function glGetActiveAttrib
%
% usage:  [ length, size, type, name ] = glGetActiveAttrib( program, index, bufSize )
%
% C function:  void glGetActiveAttrib(GLuint program, GLuint index, GLsizei bufSize, GLsizei* length, GLsizei* size, GLenum* type, GLchar* name)

% 05-Mar-2006 -- created (generated automatically from header files)

% ---allocate---

if nargin~=3,
    error('invalid number of arguments');
end

length = int32(0);
size = int32(0);
type = uint32(0);
name = uint8(0);

moglcore( 'glGetActiveAttrib', program, index, bufSize, length, size, type, name );

return
