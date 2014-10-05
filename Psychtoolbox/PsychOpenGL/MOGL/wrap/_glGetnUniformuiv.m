function params = glGetnUniformuiv( program, location, bufSize )

% glGetnUniformuiv  Interface to OpenGL function glGetnUniformuiv
%
% usage:  params = glGetnUniformuiv( program, location, bufSize )
%
% C function:  void glGetnUniformuiv(GLuint program, GLint location, GLsizei bufSize, GLuint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=3,
    error('invalid number of arguments');
end

params = uint32(0);

moglcore( 'glGetnUniformuiv', program, location, bufSize, params );

return
