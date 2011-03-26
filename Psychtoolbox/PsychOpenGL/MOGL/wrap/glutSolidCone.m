function glutSolidCone( base, height, slices, stacks )

% glutSolidCone  Interface to OpenGL function glutSolidCone
%
% usage:  glutSolidCone( base, height, slices, stacks )
%
% C function:  void glutSolidCone(GLdouble base, GLdouble height, GLint slices, GLint stacks)

% 26-Mar-2011 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glutSolidCone', base, height, slices, stacks );

return
