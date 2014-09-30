function glVertexAttribI4ubv( index, v )

% glVertexAttribI4ubv  Interface to OpenGL function glVertexAttribI4ubv
%
% usage:  glVertexAttribI4ubv( index, v )
%
% C function:  void glVertexAttribI4ubv(GLuint index, const GLubyte* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttribI4ubv', index, uint8(v) );

return
