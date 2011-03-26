function glPolygonOffset( factor, units )

% glPolygonOffset  Interface to OpenGL function glPolygonOffset
%
% usage:  glPolygonOffset( factor, units )
%
% C function:  void glPolygonOffset(GLfloat factor, GLfloat units)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glPolygonOffset', factor, units );

return
