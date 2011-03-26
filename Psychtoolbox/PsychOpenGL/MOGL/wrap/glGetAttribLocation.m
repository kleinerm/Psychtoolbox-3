function r = glGetAttribLocation( program, name )

% glGetAttribLocation  Interface to OpenGL function glGetAttribLocation
%
% usage:  r = glGetAttribLocation( program, name )
%
% C function:  GLint glGetAttribLocation(GLuint program, const GLchar* name)

% 05-Mar-2006 -- created (generated automatically from header files)

% ---protected---

if nargin~=2,
    error('invalid number of arguments');
end

r = moglcore( 'glGetAttribLocation', program, [ uint8(name) 0 ]);

return
% ---skip---
