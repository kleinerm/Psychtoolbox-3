function params = glGetUniformiv( program, location )

% glGetUniformiv  Interface to OpenGL function glGetUniformiv
%
% usage:  params = glGetUniformiv( program, location )
%
% C function:  void glGetUniformiv(GLuint program, GLint location, GLint* params)

% 05-Mar-2006 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = int32(0);

moglcore( 'glGetUniformiv', program, location, params );

return
