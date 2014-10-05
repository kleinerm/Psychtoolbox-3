function r = glCreateProgramObjectARB

% glCreateProgramObjectARB  Interface to OpenGL function glCreateProgramObjectARB
%
% usage:  r = glCreateProgramObjectARB
%
% C function:  GLhandleARB glCreateProgramObjectARB(void)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=0,
    error('invalid number of arguments');
end

r = moglcore( 'glCreateProgramObjectARB' );

return
