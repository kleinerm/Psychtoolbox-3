function params = glGetNamedProgramLocalParameterfvEXT( program, target, index )

% glGetNamedProgramLocalParameterfvEXT  Interface to OpenGL function glGetNamedProgramLocalParameterfvEXT
%
% usage:  params = glGetNamedProgramLocalParameterfvEXT( program, target, index )
%
% C function:  void glGetNamedProgramLocalParameterfvEXT(GLuint program, GLenum target, GLuint index, GLfloat* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=3,
    error('invalid number of arguments');
end

params = single(0);

moglcore( 'glGetNamedProgramLocalParameterfvEXT', program, target, index, params );

return
