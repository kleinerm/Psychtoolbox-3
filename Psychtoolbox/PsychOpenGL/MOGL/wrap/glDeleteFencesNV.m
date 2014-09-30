function glDeleteFencesNV( n, fences )

% glDeleteFencesNV  Interface to OpenGL function glDeleteFencesNV
%
% usage:  glDeleteFencesNV( n, fences )
%
% C function:  void glDeleteFencesNV(GLsizei n, const GLuint* fences)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glDeleteFencesNV', n, uint32(fences) );

return
