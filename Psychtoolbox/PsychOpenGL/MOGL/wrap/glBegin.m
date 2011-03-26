function glBegin( mode )

% glBegin  Interface to OpenGL function glBegin
%
% usage:  glBegin( mode )
%
% C function:  void glBegin(GLenum mode)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glBegin', mode );

return
