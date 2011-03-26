function glLoadIdentity

% glLoadIdentity  Interface to OpenGL function glLoadIdentity
%
% usage:  glLoadIdentity
%
% C function:  void glLoadIdentity(void)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=0,
    error('invalid number of arguments');
end

moglcore( 'glLoadIdentity' );

return
