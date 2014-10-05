function glVertexAttribL2dvEXT( index, v )

% glVertexAttribL2dvEXT  Interface to OpenGL function glVertexAttribL2dvEXT
%
% usage:  glVertexAttribL2dvEXT( index, v )
%
% C function:  void glVertexAttribL2dvEXT(GLuint index, const GLdouble* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttribL2dvEXT', index, double(v) );

return
