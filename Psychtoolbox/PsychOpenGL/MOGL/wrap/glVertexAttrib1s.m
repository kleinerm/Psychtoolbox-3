function glVertexAttrib1s( index, x )

% glVertexAttrib1s  Interface to OpenGL function glVertexAttrib1s
%
% usage:  glVertexAttrib1s( index, x )
%
% C function:  void glVertexAttrib1s(GLuint index, GLshort x)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttrib1s', index, x );

return
