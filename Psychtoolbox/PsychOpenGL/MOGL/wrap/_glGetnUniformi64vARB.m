function params = glGetnUniformi64vARB( program, location, bufSize )

% glGetnUniformi64vARB  Interface to OpenGL function glGetnUniformi64vARB
%
% usage:  params = glGetnUniformi64vARB( program, location, bufSize )
%
% C function:  void glGetnUniformi64vARB(GLuint program, GLint location, GLsizei bufSize, GLint64* params)

% 08-Aug-2020 -- created (generated automatically from header files)

% ---allocate---

if nargin~=3,
    error('invalid number of arguments');
end

params = int64(0);

moglcore( 'glGetnUniformi64vARB', program, location, bufSize, params );

return
