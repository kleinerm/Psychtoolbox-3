function glDrawCommandsAddressNV( primitiveMode, indirects, sizes, count )

% glDrawCommandsAddressNV  Interface to OpenGL function glDrawCommandsAddressNV
%
% usage:  glDrawCommandsAddressNV( primitiveMode, indirects, sizes, count )
%
% C function:  void glDrawCommandsAddressNV(GLenum primitiveMode, const GLuint64* indirects, const GLsizei* sizes, GLuint count)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glDrawCommandsAddressNV', primitiveMode, uint64(indirects), int32(sizes), count );

return
