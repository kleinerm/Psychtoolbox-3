function glEnableVertexAttribAPPLE( index, pname )

% glEnableVertexAttribAPPLE  Interface to OpenGL function glEnableVertexAttribAPPLE
%
% usage:  glEnableVertexAttribAPPLE( index, pname )
%
% C function:  void glEnableVertexAttribAPPLE(GLuint index, GLenum pname)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glEnableVertexAttribAPPLE', index, pname );

return
