function glLinkProgramARB( programObj )

% glLinkProgramARB  Interface to OpenGL function glLinkProgramARB
%
% usage:  glLinkProgramARB( programObj )
%
% C function:  void glLinkProgramARB(GLhandleARB programObj)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glLinkProgramARB', programObj );

return
