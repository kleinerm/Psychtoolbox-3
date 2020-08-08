function glDrawCommandsNV( primitiveMode, buffer, indirects, sizes, count )

% glDrawCommandsNV  Interface to OpenGL function glDrawCommandsNV
%
% usage:  glDrawCommandsNV( primitiveMode, buffer, indirects, sizes, count )
%
% C function:  void glDrawCommandsNV(GLenum primitiveMode, GLuint buffer, const GLintptr* indirects, const GLsizei* sizes, GLuint count)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glDrawCommandsNV', primitiveMode, buffer, int64(indirects), int32(sizes), count );

return
