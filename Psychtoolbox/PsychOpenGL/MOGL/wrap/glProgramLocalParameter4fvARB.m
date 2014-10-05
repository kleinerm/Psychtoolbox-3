function glProgramLocalParameter4fvARB( target, index, params )

% glProgramLocalParameter4fvARB  Interface to OpenGL function glProgramLocalParameter4fvARB
%
% usage:  glProgramLocalParameter4fvARB( target, index, params )
%
% C function:  void glProgramLocalParameter4fvARB(GLenum target, GLuint index, const GLfloat* params)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glProgramLocalParameter4fvARB', target, index, single(params) );

return
