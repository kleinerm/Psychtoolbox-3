function glFlush

% glFlush  Interface to OpenGL function glFlush
%
% usage:  glFlush
%
% C function:  void glFlush(void)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=0,
    error('invalid number of arguments');
end

moglcore( 'glFlush' );

return
