function params = glGetNamedProgramLocalParameterIuivEXT( program, target, index )

% glGetNamedProgramLocalParameterIuivEXT  Interface to OpenGL function glGetNamedProgramLocalParameterIuivEXT
%
% usage:  params = glGetNamedProgramLocalParameterIuivEXT( program, target, index )
%
% C function:  void glGetNamedProgramLocalParameterIuivEXT(GLuint program, GLenum target, GLuint index, GLuint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=3,
    error('invalid number of arguments');
end

params = uint32(0);

moglcore( 'glGetNamedProgramLocalParameterIuivEXT', program, target, index, params );

return
