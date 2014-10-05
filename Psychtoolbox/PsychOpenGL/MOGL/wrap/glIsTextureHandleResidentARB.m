function r = glIsTextureHandleResidentARB( handle )

% glIsTextureHandleResidentARB  Interface to OpenGL function glIsTextureHandleResidentARB
%
% usage:  r = glIsTextureHandleResidentARB( handle )
%
% C function:  GLboolean glIsTextureHandleResidentARB(GLuint64 handle)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

r = moglcore( 'glIsTextureHandleResidentARB', uint64(handle) );

return
