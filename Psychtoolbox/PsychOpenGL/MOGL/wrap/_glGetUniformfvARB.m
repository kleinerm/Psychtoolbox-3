function params = glGetUniformfvARB( programObj, location )

% glGetUniformfvARB  Interface to OpenGL function glGetUniformfvARB
%
% usage:  params = glGetUniformfvARB( programObj, location )
%
% C function:  void glGetUniformfvARB(GLhandleARB programObj, GLint location, GLfloat* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = single(0);

moglcore( 'glGetUniformfvARB', programObj, location, params );

return
