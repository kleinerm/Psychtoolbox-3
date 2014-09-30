function params = glGetNamedProgramLocalParameterIivEXT( program, target, index )

% glGetNamedProgramLocalParameterIivEXT  Interface to OpenGL function glGetNamedProgramLocalParameterIivEXT
%
% usage:  params = glGetNamedProgramLocalParameterIivEXT( program, target, index )
%
% C function:  void glGetNamedProgramLocalParameterIivEXT(GLuint program, GLenum target, GLuint index, GLint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=3,
    error('invalid number of arguments');
end

params = int32(0);

moglcore( 'glGetNamedProgramLocalParameterIivEXT', program, target, index, params );

return
