function glDeleteStatesNV( n, states )

% glDeleteStatesNV  Interface to OpenGL function glDeleteStatesNV
%
% usage:  glDeleteStatesNV( n, states )
%
% C function:  void glDeleteStatesNV(GLsizei n, const GLuint* states)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glDeleteStatesNV', n, uint32(states) );

return
