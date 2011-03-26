function glMatrixMode( mode )

% glMatrixMode  Interface to OpenGL function glMatrixMode
%
% usage:  glMatrixMode( mode )
%
% C function:  void glMatrixMode(GLenum mode)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glMatrixMode', mode );

return
