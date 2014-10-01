function glDeleteFencesAPPLE( n, fences )

% glDeleteFencesAPPLE  Interface to OpenGL function glDeleteFencesAPPLE
%
% usage:  glDeleteFencesAPPLE( n, fences )
%
% C function:  void glDeleteFencesAPPLE(GLsizei n, const GLuint* fences)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glDeleteFencesAPPLE', n, uint32(fences) );

return
