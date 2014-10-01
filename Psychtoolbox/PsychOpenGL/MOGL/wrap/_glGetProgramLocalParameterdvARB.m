function params = glGetProgramLocalParameterdvARB( target, index )

% glGetProgramLocalParameterdvARB  Interface to OpenGL function glGetProgramLocalParameterdvARB
%
% usage:  params = glGetProgramLocalParameterdvARB( target, index )
%
% C function:  void glGetProgramLocalParameterdvARB(GLenum target, GLuint index, GLdouble* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = double(0);

moglcore( 'glGetProgramLocalParameterdvARB', target, index, params );

return
