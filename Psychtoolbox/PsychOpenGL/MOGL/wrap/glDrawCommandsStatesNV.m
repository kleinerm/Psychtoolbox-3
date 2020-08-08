function glDrawCommandsStatesNV( buffer, indirects, sizes, states, fbos, count )

% glDrawCommandsStatesNV  Interface to OpenGL function glDrawCommandsStatesNV
%
% usage:  glDrawCommandsStatesNV( buffer, indirects, sizes, states, fbos, count )
%
% C function:  void glDrawCommandsStatesNV(GLuint buffer, const GLintptr* indirects, const GLsizei* sizes, const GLuint* states, const GLuint* fbos, GLuint count)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=6,
    error('invalid number of arguments');
end

moglcore( 'glDrawCommandsStatesNV', buffer, int64(indirects), int32(sizes), uint32(states), uint32(fbos), count );

return
