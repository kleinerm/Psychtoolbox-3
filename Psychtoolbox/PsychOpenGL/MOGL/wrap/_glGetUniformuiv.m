function params = glGetUniformuiv( program, location )

% glGetUniformuiv  Interface to OpenGL function glGetUniformuiv
%
% usage:  params = glGetUniformuiv( program, location )
%
% C function:  void glGetUniformuiv(GLuint program, GLint location, GLuint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = uint32(0);

moglcore( 'glGetUniformuiv', program, location, params );

return
