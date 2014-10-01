function glVertexWeightfvEXT( weight )

% glVertexWeightfvEXT  Interface to OpenGL function glVertexWeightfvEXT
%
% usage:  glVertexWeightfvEXT( weight )
%
% C function:  void glVertexWeightfvEXT(const GLfloat* weight)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glVertexWeightfvEXT', single(weight) );

return
