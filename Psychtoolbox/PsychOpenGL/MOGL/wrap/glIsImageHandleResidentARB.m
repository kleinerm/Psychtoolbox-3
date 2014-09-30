function r = glIsImageHandleResidentARB( handle )

% glIsImageHandleResidentARB  Interface to OpenGL function glIsImageHandleResidentARB
%
% usage:  r = glIsImageHandleResidentARB( handle )
%
% C function:  GLboolean glIsImageHandleResidentARB(GLuint64 handle)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

r = moglcore( 'glIsImageHandleResidentARB', uint64(handle) );

return
