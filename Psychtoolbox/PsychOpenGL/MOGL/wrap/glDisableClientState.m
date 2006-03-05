function glDisableClientState( array )

% glDisableClientState  Interface to OpenGL function glDisableClientState
%
% usage:  glDisableClientState( array )
%
% C function:  void glDisableClientState(GLenum array)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glDisableClientState', array );

return
