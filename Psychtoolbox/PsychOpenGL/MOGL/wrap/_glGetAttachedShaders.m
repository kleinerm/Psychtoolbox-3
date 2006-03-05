function [ count, shaders ] = glGetAttachedShaders( program, maxCount )

% glGetAttachedShaders  Interface to OpenGL function glGetAttachedShaders
%
% usage:  [ count, shaders ] = glGetAttachedShaders( program, maxCount )
%
% C function:  void glGetAttachedShaders(GLuint program, GLsizei maxCount, GLsizei* count, GLuint* shaders)

% 05-Mar-2006 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

count = int32(0);
shaders = uint32(0);

moglcore( 'glGetAttachedShaders', program, maxCount, count, shaders );

return
