function glMakeTextureHandleNonResidentNV( handle )

% glMakeTextureHandleNonResidentNV  Interface to OpenGL function glMakeTextureHandleNonResidentNV
%
% usage:  glMakeTextureHandleNonResidentNV( handle )
%
% C function:  void glMakeTextureHandleNonResidentNV(GLuint64 handle)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glMakeTextureHandleNonResidentNV', uint64(handle) );

return
