function glMakeTextureHandleResidentNV( handle )

% glMakeTextureHandleResidentNV  Interface to OpenGL function glMakeTextureHandleResidentNV
%
% usage:  glMakeTextureHandleResidentNV( handle )
%
% C function:  void glMakeTextureHandleResidentNV(GLuint64 handle)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glMakeTextureHandleResidentNV', uint64(handle) );

return
