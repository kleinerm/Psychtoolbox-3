function r = glIsProgramARB( program )

% glIsProgramARB  Interface to OpenGL function glIsProgramARB
%
% usage:  r = glIsProgramARB( program )
%
% C function:  GLboolean glIsProgramARB(GLuint program)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

r = moglcore( 'glIsProgramARB', program );

return
