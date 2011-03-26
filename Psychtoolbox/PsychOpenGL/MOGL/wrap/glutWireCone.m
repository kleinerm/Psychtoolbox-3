function glutWireCone( base, height, slices, stacks )

% glutWireCone  Interface to OpenGL function glutWireCone
%
% usage:  glutWireCone( base, height, slices, stacks )
%
% C function:  void glutWireCone(GLdouble base, GLdouble height, GLint slices, GLint stacks)

% 26-Mar-2011 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glutWireCone', base, height, slices, stacks );

return
