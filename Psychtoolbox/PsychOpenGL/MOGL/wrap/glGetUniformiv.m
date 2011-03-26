function params = glGetUniformiv( program, location )

% glGetUniformiv  Interface to OpenGL function glGetUniformiv
%
% usage:  params = glGetUniformiv( program, location )
%
% C function:  void glGetUniformiv(GLuint program, GLint location, GLint* params)

% 05-Mar-2006 -- created (generated automatically from header files)

% ---allocate---
% ---protected---

if nargin~=2,
    error('invalid number of arguments');
end

% Allocate enough bufferspace for the biggest returned vector:
params=int32(repmat(intmax, [ 16 1 ]));

moglcore( 'glGetUniformiv', program, location, params );
params = params(find(params~=intmax));

return
% ---skip---
