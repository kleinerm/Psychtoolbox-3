function glEnableClientState( array )

% glEnableClientState  Interface to OpenGL function glEnableClientState
%
% usage:  glEnableClientState( array )
%
% C function:  void glEnableClientState(GLenum array)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glEnableClientState', array );

return
