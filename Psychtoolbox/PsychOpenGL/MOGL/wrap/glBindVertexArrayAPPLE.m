function glBindVertexArrayAPPLE( array )

% glBindVertexArrayAPPLE  Interface to OpenGL function glBindVertexArrayAPPLE
%
% usage:  glBindVertexArrayAPPLE( array )
%
% C function:  void glBindVertexArrayAPPLE(GLuint array)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glBindVertexArrayAPPLE', array );

return
