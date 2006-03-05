function glPolygonOffset( factor, units )

% glPolygonOffset  Interface to OpenGL function glPolygonOffset
%
% usage:  glPolygonOffset( factor, units )
%
% C function:  void glPolygonOffset(GLfloat factor, GLfloat units)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glPolygonOffset', factor, units );

return
