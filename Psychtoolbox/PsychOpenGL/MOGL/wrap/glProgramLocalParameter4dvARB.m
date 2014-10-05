function glProgramLocalParameter4dvARB( target, index, params )

% glProgramLocalParameter4dvARB  Interface to OpenGL function glProgramLocalParameter4dvARB
%
% usage:  glProgramLocalParameter4dvARB( target, index, params )
%
% C function:  void glProgramLocalParameter4dvARB(GLenum target, GLuint index, const GLdouble* params)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glProgramLocalParameter4dvARB', target, index, double(params) );

return
