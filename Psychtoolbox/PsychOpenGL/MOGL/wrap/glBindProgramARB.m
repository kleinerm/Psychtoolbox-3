function glBindProgramARB( target, program )

% glBindProgramARB  Interface to OpenGL function glBindProgramARB
%
% usage:  glBindProgramARB( target, program )
%
% C function:  void glBindProgramARB(GLenum target, GLuint program)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glBindProgramARB', target, program );

return
