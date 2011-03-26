function glReadBuffer( mode )

% glReadBuffer  Interface to OpenGL function glReadBuffer
%
% usage:  glReadBuffer( mode )
%
% C function:  void glReadBuffer(GLenum mode)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glReadBuffer', mode );

return
