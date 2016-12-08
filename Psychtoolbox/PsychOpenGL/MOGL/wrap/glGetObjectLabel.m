function [ length, label ] = glGetObjectLabel( identifier, name, bufSize )

% glGetObjectLabel  Interface to OpenGL function glGetObjectLabel
%
% usage:  [ length, label ] = glGetObjectLabel( identifier, name, bufSize )
%
% C function:  void glGetObjectLabel(GLenum identifier, GLuint name, GLsizei bufSize, GLsizei* length, GLchar* label)

% 30-Aug-2012 -- created (generated automatically from header files)

% ---allocate---
% ---protected---

if nargin~=3,
    error('invalid number of arguments');
end

length = int32([0,0]);
label = uint8(zeros(1, bufSize+1));

moglcore( 'glGetObjectLabel', identifier, name, bufSize, length, label );
label = char(label(1:end-1));
length = length(1:end-1);
return
