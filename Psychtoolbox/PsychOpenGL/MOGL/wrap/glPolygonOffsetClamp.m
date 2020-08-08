function glPolygonOffsetClamp( factor, units, clamp )

% glPolygonOffsetClamp  Interface to OpenGL function glPolygonOffsetClamp
%
% usage:  glPolygonOffsetClamp( factor, units, clamp )
%
% C function:  void glPolygonOffsetClamp(GLfloat factor, GLfloat units, GLfloat clamp)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glPolygonOffsetClamp', factor, units, clamp );

return
