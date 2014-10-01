function [ length, size, type, name ] = glGetTransformFeedbackVaryingEXT( program, index, bufSize )

% glGetTransformFeedbackVaryingEXT  Interface to OpenGL function glGetTransformFeedbackVaryingEXT
%
% usage:  [ length, size, type, name ] = glGetTransformFeedbackVaryingEXT( program, index, bufSize )
%
% C function:  void glGetTransformFeedbackVaryingEXT(GLuint program, GLuint index, GLsizei bufSize, GLsizei* length, GLsizei* size, GLenum* type, GLchar* name)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---
% ---protected---

if nargin~=3,
    error('invalid number of arguments');
end

length = int32(0);
size = int32(0);
type = uint32(0);
name = uint8(zeros(1, bufSize));

moglcore( 'glGetTransformFeedbackVaryingEXT', program, index, bufSize, length, size, type, name );
name = char(name(1:length));

return
