function params = glGetnUniformiv( program, location, bufSize )

% glGetnUniformiv  Interface to OpenGL function glGetnUniformiv
%
% usage:  params = glGetnUniformiv( program, location, bufSize )
%
% C function:  void glGetnUniformiv(GLuint program, GLint location, GLsizei bufSize, GLint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=3,
    error('invalid number of arguments');
end

params = int32(0);

moglcore( 'glGetnUniformiv', program, location, bufSize, params );

return
