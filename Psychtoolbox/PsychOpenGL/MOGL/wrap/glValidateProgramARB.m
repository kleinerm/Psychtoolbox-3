function glValidateProgramARB( programObj )

% glValidateProgramARB  Interface to OpenGL function glValidateProgramARB
%
% usage:  glValidateProgramARB( programObj )
%
% C function:  void glValidateProgramARB(GLhandleARB programObj)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glValidateProgramARB', programObj );

return
