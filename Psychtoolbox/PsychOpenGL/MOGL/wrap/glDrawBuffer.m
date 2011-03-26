function glDrawBuffer( mode )

% glDrawBuffer  Interface to OpenGL function glDrawBuffer
%
% usage:  glDrawBuffer( mode )
%
% C function:  void glDrawBuffer(GLenum mode)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glDrawBuffer', mode );

return
