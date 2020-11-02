function params = glGetUniformui64vARB( program, location )

% glGetUniformui64vARB  Interface to OpenGL function glGetUniformui64vARB
%
% usage:  params = glGetUniformui64vARB( program, location )
%
% C function:  void glGetUniformui64vARB(GLuint program, GLint location, GLuint64* params)

% 08-Aug-2020 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = uint64(0);

moglcore( 'glGetUniformui64vARB', program, location, params );

return
