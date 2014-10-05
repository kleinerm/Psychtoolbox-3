function glProgramEnvParameter4dvARB( target, index, params )

% glProgramEnvParameter4dvARB  Interface to OpenGL function glProgramEnvParameter4dvARB
%
% usage:  glProgramEnvParameter4dvARB( target, index, params )
%
% C function:  void glProgramEnvParameter4dvARB(GLenum target, GLuint index, const GLdouble* params)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glProgramEnvParameter4dvARB', target, index, double(params) );

return
