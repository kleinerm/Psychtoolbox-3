function glMultiModeDrawElementsIBM( mode, count, type, const, primcount, modestride )

% glMultiModeDrawElementsIBM  Interface to OpenGL function glMultiModeDrawElementsIBM
%
% usage:  glMultiModeDrawElementsIBM( mode, count, type, const, primcount, modestride )
%
% C function:  void glMultiModeDrawElementsIBM(const GLenum* mode, const GLsizei* count, GLenum type, const void* const, GLsizei primcount, GLint modestride)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=6,
    error('invalid number of arguments');
end

moglcore( 'glMultiModeDrawElementsIBM', uint32(mode), int32(count), type, const, primcount, modestride );

return
