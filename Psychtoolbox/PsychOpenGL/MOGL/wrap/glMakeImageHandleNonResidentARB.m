function glMakeImageHandleNonResidentARB( handle )

% glMakeImageHandleNonResidentARB  Interface to OpenGL function glMakeImageHandleNonResidentARB
%
% usage:  glMakeImageHandleNonResidentARB( handle )
%
% C function:  void glMakeImageHandleNonResidentARB(GLuint64 handle)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glMakeImageHandleNonResidentARB', uint64(handle) );

return
