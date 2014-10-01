function [ length, label ] = glGetObjectLabelEXT( type, object, bufSize )

% glGetObjectLabelEXT  Interface to OpenGL function glGetObjectLabelEXT
%
% usage:  [ length, label ] = glGetObjectLabelEXT( type, object, bufSize )
%
% C function:  void glGetObjectLabelEXT(GLenum type, GLuint object, GLsizei bufSize, GLsizei* length, GLchar* label)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=3,
    error('invalid number of arguments');
end

length = int32(0);
label = uint8(0);

moglcore( 'glGetObjectLabelEXT', type, object, bufSize, length, label );

return
