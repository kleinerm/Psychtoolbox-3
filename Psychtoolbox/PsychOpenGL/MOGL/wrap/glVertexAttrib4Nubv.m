function glVertexAttrib4Nubv( index, v )

% glVertexAttrib4Nubv  Interface to OpenGL function glVertexAttrib4Nubv
%
% usage:  glVertexAttrib4Nubv( index, v )
%
% C function:  void glVertexAttrib4Nubv(GLuint index, const GLubyte* v)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttrib4Nubv', index, uint8(v) );

return
