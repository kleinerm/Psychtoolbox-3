function values = glGetActiveSubroutineUniformiv( program, shadertype, index, pname )

% glGetActiveSubroutineUniformiv  Interface to OpenGL function glGetActiveSubroutineUniformiv
%
% usage:  values = glGetActiveSubroutineUniformiv( program, shadertype, index, pname )
%
% C function:  void glGetActiveSubroutineUniformiv(GLuint program, GLenum shadertype, GLuint index, GLenum pname, GLint* values)

% 30-Aug-2012 -- created (generated automatically from header files)

% ---allocate---
% ---protected---

if nargin~=4,
    error('invalid number of arguments');
end

values = int32(0);

moglcore( 'glGetActiveSubroutineUniformiv', program, shadertype, index, pname, values );

return
