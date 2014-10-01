function params = glGetProgramEnvParameterfvARB( target, index )

% glGetProgramEnvParameterfvARB  Interface to OpenGL function glGetProgramEnvParameterfvARB
%
% usage:  params = glGetProgramEnvParameterfvARB( target, index )
%
% C function:  void glGetProgramEnvParameterfvARB(GLenum target, GLuint index, GLfloat* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = single(0);

moglcore( 'glGetProgramEnvParameterfvARB', target, index, params );

return
