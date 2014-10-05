function glMultiModeDrawArraysIBM( mode, first, count, primcount, modestride )

% glMultiModeDrawArraysIBM  Interface to OpenGL function glMultiModeDrawArraysIBM
%
% usage:  glMultiModeDrawArraysIBM( mode, first, count, primcount, modestride )
%
% C function:  void glMultiModeDrawArraysIBM(const GLenum* mode, const GLint* first, const GLsizei* count, GLsizei primcount, GLint modestride)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glMultiModeDrawArraysIBM', uint32(mode), int32(first), int32(count), primcount, modestride );

return
