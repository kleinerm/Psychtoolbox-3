function glVertexWeightfEXT( weight )

% glVertexWeightfEXT  Interface to OpenGL function glVertexWeightfEXT
%
% usage:  glVertexWeightfEXT( weight )
%
% C function:  void glVertexWeightfEXT(GLfloat weight)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glVertexWeightfEXT', weight );

return
