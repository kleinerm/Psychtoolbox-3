function params = glGetUniformi64vARB( program, location )

% glGetUniformi64vARB  Interface to OpenGL function glGetUniformi64vARB
%
% usage:  params = glGetUniformi64vARB( program, location )
%
% C function:  void glGetUniformi64vARB(GLuint program, GLint location, GLint64* params)

% 08-Aug-2020 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = int64(0);

moglcore( 'glGetUniformi64vARB', program, location, params );

return
