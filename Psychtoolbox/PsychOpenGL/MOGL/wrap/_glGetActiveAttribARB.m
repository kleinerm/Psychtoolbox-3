function [ length, size, type, name ] = glGetActiveAttribARB( programObj, index, maxLength )

% glGetActiveAttribARB  Interface to OpenGL function glGetActiveAttribARB
%
% usage:  [ length, size, type, name ] = glGetActiveAttribARB( programObj, index, maxLength )
%
% C function:  void glGetActiveAttribARB(GLhandleARB programObj, GLuint index, GLsizei maxLength, GLsizei* length, GLint* size, GLenum* type, GLcharARB* name)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=3,
    error('invalid number of arguments');
end

length = int32(0);
size = int32(0);
type = uint32(0);
name = uint8(0);

moglcore( 'glGetActiveAttribARB', programObj, index, maxLength, length, size, type, name );

return
