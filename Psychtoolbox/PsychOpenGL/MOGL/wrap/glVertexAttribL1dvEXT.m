function glVertexAttribL1dvEXT( index, v )

% glVertexAttribL1dvEXT  Interface to OpenGL function glVertexAttribL1dvEXT
%
% usage:  glVertexAttribL1dvEXT( index, v )
%
% C function:  void glVertexAttribL1dvEXT(GLuint index, const GLdouble* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttribL1dvEXT', index, double(v) );

return
