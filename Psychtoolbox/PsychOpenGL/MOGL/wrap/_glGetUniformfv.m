function params = glGetUniformfv( program, location )

% glGetUniformfv  Interface to OpenGL function glGetUniformfv
%
% usage:  params = glGetUniformfv( program, location )
%
% C function:  void glGetUniformfv(GLuint program, GLint location, GLfloat* params)

% 05-Mar-2006 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = moglsingle(0);

moglcore( 'glGetUniformfv', program, location, params );

return
