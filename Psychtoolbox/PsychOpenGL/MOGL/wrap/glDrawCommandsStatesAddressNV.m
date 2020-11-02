function glDrawCommandsStatesAddressNV( indirects, sizes, states, fbos, count )

% glDrawCommandsStatesAddressNV  Interface to OpenGL function glDrawCommandsStatesAddressNV
%
% usage:  glDrawCommandsStatesAddressNV( indirects, sizes, states, fbos, count )
%
% C function:  void glDrawCommandsStatesAddressNV(const GLuint64* indirects, const GLsizei* sizes, const GLuint* states, const GLuint* fbos, GLuint count)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glDrawCommandsStatesAddressNV', uint64(indirects), int32(sizes), uint32(states), uint32(fbos), count );

return
