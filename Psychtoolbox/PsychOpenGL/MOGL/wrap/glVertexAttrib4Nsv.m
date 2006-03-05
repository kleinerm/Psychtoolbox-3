function glVertexAttrib4Nsv( index, v )

% glVertexAttrib4Nsv  Interface to OpenGL function glVertexAttrib4Nsv
%
% usage:  glVertexAttrib4Nsv( index, v )
%
% C function:  void glVertexAttrib4Nsv(GLuint index, const GLshort* v)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttrib4Nsv', index, int16(v) );

return
