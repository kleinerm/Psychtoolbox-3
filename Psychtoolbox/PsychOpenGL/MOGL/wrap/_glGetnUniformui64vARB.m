function params = glGetnUniformui64vARB( program, location, bufSize )

% glGetnUniformui64vARB  Interface to OpenGL function glGetnUniformui64vARB
%
% usage:  params = glGetnUniformui64vARB( program, location, bufSize )
%
% C function:  void glGetnUniformui64vARB(GLuint program, GLint location, GLsizei bufSize, GLuint64* params)

% 08-Aug-2020 -- created (generated automatically from header files)

% ---allocate---

if nargin~=3,
    error('invalid number of arguments');
end

params = uint64(0);

moglcore( 'glGetnUniformui64vARB', program, location, bufSize, params );

return
