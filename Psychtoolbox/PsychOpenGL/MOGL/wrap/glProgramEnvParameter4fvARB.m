function glProgramEnvParameter4fvARB( target, index, params )

% glProgramEnvParameter4fvARB  Interface to OpenGL function glProgramEnvParameter4fvARB
%
% usage:  glProgramEnvParameter4fvARB( target, index, params )
%
% C function:  void glProgramEnvParameter4fvARB(GLenum target, GLuint index, const GLfloat* params)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glProgramEnvParameter4fvARB', target, index, single(params) );

return
