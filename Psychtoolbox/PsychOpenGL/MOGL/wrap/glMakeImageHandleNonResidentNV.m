function glMakeImageHandleNonResidentNV( handle )

% glMakeImageHandleNonResidentNV  Interface to OpenGL function glMakeImageHandleNonResidentNV
%
% usage:  glMakeImageHandleNonResidentNV( handle )
%
% C function:  void glMakeImageHandleNonResidentNV(GLuint64 handle)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glMakeImageHandleNonResidentNV', uint64(handle) );

return
