function glVertexAttrib4ubv( index, v )

% glVertexAttrib4ubv  Interface to OpenGL function glVertexAttrib4ubv
%
% usage:  glVertexAttrib4ubv( index, v )
%
% C function:  void glVertexAttrib4ubv(GLuint index, const GLubyte* v)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttrib4ubv', index, uint8(v) );

return
