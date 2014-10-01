function glMakeTextureHandleResidentARB( handle )

% glMakeTextureHandleResidentARB  Interface to OpenGL function glMakeTextureHandleResidentARB
%
% usage:  glMakeTextureHandleResidentARB( handle )
%
% C function:  void glMakeTextureHandleResidentARB(GLuint64 handle)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glMakeTextureHandleResidentARB', uint64(handle) );

return
