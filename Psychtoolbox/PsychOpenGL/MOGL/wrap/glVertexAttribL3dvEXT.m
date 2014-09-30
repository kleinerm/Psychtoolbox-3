function glVertexAttribL3dvEXT( index, v )

% glVertexAttribL3dvEXT  Interface to OpenGL function glVertexAttribL3dvEXT
%
% usage:  glVertexAttribL3dvEXT( index, v )
%
% C function:  void glVertexAttribL3dvEXT(GLuint index, const GLdouble* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttribL3dvEXT', index, double(v) );

return
