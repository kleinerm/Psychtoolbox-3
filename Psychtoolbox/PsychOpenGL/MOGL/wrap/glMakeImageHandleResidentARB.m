function glMakeImageHandleResidentARB( handle, access )

% glMakeImageHandleResidentARB  Interface to OpenGL function glMakeImageHandleResidentARB
%
% usage:  glMakeImageHandleResidentARB( handle, access )
%
% C function:  void glMakeImageHandleResidentARB(GLuint64 handle, GLenum access)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glMakeImageHandleResidentARB', uint64(handle), access );

return
