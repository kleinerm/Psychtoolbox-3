function program = glGetProgramStringNV( id, pname )

% glGetProgramStringNV  Interface to OpenGL function glGetProgramStringNV
%
% usage:  program = glGetProgramStringNV( id, pname )
%
% C function:  void glGetProgramStringNV(GLuint id, GLenum pname, GLubyte* program)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

program = uint8(0);

moglcore( 'glGetProgramStringNV', id, pname, program );

return
