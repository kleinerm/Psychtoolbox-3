function glDisableVertexAttribAPPLE( index, pname )

% glDisableVertexAttribAPPLE  Interface to OpenGL function glDisableVertexAttribAPPLE
%
% usage:  glDisableVertexAttribAPPLE( index, pname )
%
% C function:  void glDisableVertexAttribAPPLE(GLuint index, GLenum pname)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glDisableVertexAttribAPPLE', index, pname );

return
