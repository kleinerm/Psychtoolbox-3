function params = glGetNamedProgramLocalParameterdvEXT( program, target, index )

% glGetNamedProgramLocalParameterdvEXT  Interface to OpenGL function glGetNamedProgramLocalParameterdvEXT
%
% usage:  params = glGetNamedProgramLocalParameterdvEXT( program, target, index )
%
% C function:  void glGetNamedProgramLocalParameterdvEXT(GLuint program, GLenum target, GLuint index, GLdouble* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=3,
    error('invalid number of arguments');
end

params = double(0);

moglcore( 'glGetNamedProgramLocalParameterdvEXT', program, target, index, params );

return
