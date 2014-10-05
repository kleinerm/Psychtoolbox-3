function glUseProgramObjectARB( programObj )

% glUseProgramObjectARB  Interface to OpenGL function glUseProgramObjectARB
%
% usage:  glUseProgramObjectARB( programObj )
%
% C function:  void glUseProgramObjectARB(GLhandleARB programObj)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glUseProgramObjectARB', programObj );

return
