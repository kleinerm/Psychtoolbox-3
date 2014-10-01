function r = glIsImageHandleResidentNV( handle )

% glIsImageHandleResidentNV  Interface to OpenGL function glIsImageHandleResidentNV
%
% usage:  r = glIsImageHandleResidentNV( handle )
%
% C function:  GLboolean glIsImageHandleResidentNV(GLuint64 handle)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

r = moglcore( 'glIsImageHandleResidentNV', uint64(handle) );

return
