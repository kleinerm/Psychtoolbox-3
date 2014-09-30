function glLoadProgramNV( target, id, len, program )

% glLoadProgramNV  Interface to OpenGL function glLoadProgramNV
%
% usage:  glLoadProgramNV( target, id, len, program )
%
% C function:  void glLoadProgramNV(GLenum target, GLuint id, GLsizei len, const GLubyte* program)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glLoadProgramNV', target, id, len, uint8(program) );

return
