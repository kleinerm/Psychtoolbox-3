function glVertexAttrib2s( index, x, y )

% glVertexAttrib2s  Interface to OpenGL function glVertexAttrib2s
%
% usage:  glVertexAttrib2s( index, x, y )
%
% C function:  void glVertexAttrib2s(GLuint index, GLshort x, GLshort y)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttrib2s', index, x, y );

return
