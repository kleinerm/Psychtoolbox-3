function params = glGetProgramLocalParameterfvARB( target, index )

% glGetProgramLocalParameterfvARB  Interface to OpenGL function glGetProgramLocalParameterfvARB
%
% usage:  params = glGetProgramLocalParameterfvARB( target, index )
%
% C function:  void glGetProgramLocalParameterfvARB(GLenum target, GLuint index, GLfloat* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = single(0);

moglcore( 'glGetProgramLocalParameterfvARB', target, index, params );

return
