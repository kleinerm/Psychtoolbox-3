function r = glIsTextureHandleResidentNV( handle )

% glIsTextureHandleResidentNV  Interface to OpenGL function glIsTextureHandleResidentNV
%
% usage:  r = glIsTextureHandleResidentNV( handle )
%
% C function:  GLboolean glIsTextureHandleResidentNV(GLuint64 handle)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

r = moglcore( 'glIsTextureHandleResidentNV', uint64(handle) );

return
