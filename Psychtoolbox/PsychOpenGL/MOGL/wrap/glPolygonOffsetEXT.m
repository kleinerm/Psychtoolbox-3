function glPolygonOffsetEXT( factor, bias )

% glPolygonOffsetEXT  Interface to OpenGL function glPolygonOffsetEXT
%
% usage:  glPolygonOffsetEXT( factor, bias )
%
% C function:  void glPolygonOffsetEXT(GLfloat factor, GLfloat bias)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glPolygonOffsetEXT', factor, bias );

return
