function glMakeImageHandleResidentNV( handle, access )

% glMakeImageHandleResidentNV  Interface to OpenGL function glMakeImageHandleResidentNV
%
% usage:  glMakeImageHandleResidentNV( handle, access )
%
% C function:  void glMakeImageHandleResidentNV(GLuint64 handle, GLenum access)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glMakeImageHandleResidentNV', uint64(handle), access );

return
