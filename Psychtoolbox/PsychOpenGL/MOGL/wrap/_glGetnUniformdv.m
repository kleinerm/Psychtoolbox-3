function params = glGetnUniformdv( program, location, bufSize )

% glGetnUniformdv  Interface to OpenGL function glGetnUniformdv
%
% usage:  params = glGetnUniformdv( program, location, bufSize )
%
% C function:  void glGetnUniformdv(GLuint program, GLint location, GLsizei bufSize, GLdouble* params)

% 08-Aug-2020 -- created (generated automatically from header files)

% ---allocate---

if nargin~=3,
    error('invalid number of arguments');
end

params = double(0);

moglcore( 'glGetnUniformdv', program, location, bufSize, params );

return
