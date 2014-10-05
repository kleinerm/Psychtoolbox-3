function params = glGetProgramEnvParameterdvARB( target, index )

% glGetProgramEnvParameterdvARB  Interface to OpenGL function glGetProgramEnvParameterdvARB
%
% usage:  params = glGetProgramEnvParameterdvARB( target, index )
%
% C function:  void glGetProgramEnvParameterdvARB(GLenum target, GLuint index, GLdouble* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = double(0);

moglcore( 'glGetProgramEnvParameterdvARB', target, index, params );

return
