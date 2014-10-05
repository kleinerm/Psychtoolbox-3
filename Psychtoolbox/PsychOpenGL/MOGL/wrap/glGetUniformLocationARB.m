function r = glGetUniformLocationARB( programObj, name )

% glGetUniformLocationARB  Interface to OpenGL function glGetUniformLocationARB
%
% usage:  r = glGetUniformLocationARB( programObj, name )
%
% C function:  GLint glGetUniformLocationARB(GLhandleARB programObj, const GLcharARB* name)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

r = moglcore( 'glGetUniformLocationARB', programObj, uint8(name) );

return
