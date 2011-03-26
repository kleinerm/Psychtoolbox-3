function r = glGetUniformLocation( program, name )

% glGetUniformLocation  Interface to OpenGL function glGetUniformLocation
%
% usage:  r = glGetUniformLocation( program, name )
%
% C function:  GLint glGetUniformLocation(GLuint program, const GLchar* name)

% 05-Mar-2006 -- created (generated automatically from header files)

% ---protected---

if nargin~=2,
    error('invalid number of arguments');
end

r = moglcore( 'glGetUniformLocation', program, [uint8(name) 0]);

return
% ---skip---
