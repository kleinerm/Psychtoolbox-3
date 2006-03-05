function glEnableClientState( array )

% glEnableClientState  Interface to OpenGL function glEnableClientState
%
% usage:  glEnableClientState( array )
%
% C function:  void glEnableClientState(GLenum array)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glEnableClientState', array );

return
