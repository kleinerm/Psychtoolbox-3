function glVertexAttribL4dvEXT( index, v )

% glVertexAttribL4dvEXT  Interface to OpenGL function glVertexAttribL4dvEXT
%
% usage:  glVertexAttribL4dvEXT( index, v )
%
% C function:  void glVertexAttribL4dvEXT(GLuint index, const GLdouble* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttribL4dvEXT', index, double(v) );

return
