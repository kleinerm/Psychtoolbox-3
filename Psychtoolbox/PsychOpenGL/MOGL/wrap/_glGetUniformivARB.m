function params = glGetUniformivARB( programObj, location )

% glGetUniformivARB  Interface to OpenGL function glGetUniformivARB
%
% usage:  params = glGetUniformivARB( programObj, location )
%
% C function:  void glGetUniformivARB(GLhandleARB programObj, GLint location, GLint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = int32(0);

moglcore( 'glGetUniformivARB', programObj, location, params );

return
