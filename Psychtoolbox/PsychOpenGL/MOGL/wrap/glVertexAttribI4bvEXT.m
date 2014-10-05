function glVertexAttribI4bvEXT( index, v )

% glVertexAttribI4bvEXT  Interface to OpenGL function glVertexAttribI4bvEXT
%
% usage:  glVertexAttribI4bvEXT( index, v )
%
% C function:  void glVertexAttribI4bvEXT(GLuint index, const GLbyte* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttribI4bvEXT', index, int8(v) );

return
