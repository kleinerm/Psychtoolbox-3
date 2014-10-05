function params = glGetnUniformfv( program, location, bufSize )

% glGetnUniformfv  Interface to OpenGL function glGetnUniformfv
%
% usage:  params = glGetnUniformfv( program, location, bufSize )
%
% C function:  void glGetnUniformfv(GLuint program, GLint location, GLsizei bufSize, GLfloat* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=3,
    error('invalid number of arguments');
end

params = single(0);

moglcore( 'glGetnUniformfv', program, location, bufSize, params );

return
