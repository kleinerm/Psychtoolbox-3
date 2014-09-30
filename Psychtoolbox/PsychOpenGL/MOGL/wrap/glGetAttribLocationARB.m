function r = glGetAttribLocationARB( programObj, name )

% glGetAttribLocationARB  Interface to OpenGL function glGetAttribLocationARB
%
% usage:  r = glGetAttribLocationARB( programObj, name )
%
% C function:  GLint glGetAttribLocationARB(GLhandleARB programObj, const GLcharARB* name)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

r = moglcore( 'glGetAttribLocationARB', programObj, uint8(name) );

return
