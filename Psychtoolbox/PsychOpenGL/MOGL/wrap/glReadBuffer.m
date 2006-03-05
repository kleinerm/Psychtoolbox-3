function glReadBuffer( mode )

% glReadBuffer  Interface to OpenGL function glReadBuffer
%
% usage:  glReadBuffer( mode )
%
% C function:  void glReadBuffer(GLenum mode)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glReadBuffer', mode );

return
