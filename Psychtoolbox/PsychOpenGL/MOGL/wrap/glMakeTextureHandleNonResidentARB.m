function glMakeTextureHandleNonResidentARB( handle )

% glMakeTextureHandleNonResidentARB  Interface to OpenGL function glMakeTextureHandleNonResidentARB
%
% usage:  glMakeTextureHandleNonResidentARB( handle )
%
% C function:  void glMakeTextureHandleNonResidentARB(GLuint64 handle)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glMakeTextureHandleNonResidentARB', uint64(handle) );

return
