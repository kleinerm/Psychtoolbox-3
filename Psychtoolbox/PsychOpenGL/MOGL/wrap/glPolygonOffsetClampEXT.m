function glPolygonOffsetClampEXT( factor, units, clamp )

% glPolygonOffsetClampEXT  Interface to OpenGL function glPolygonOffsetClampEXT
%
% usage:  glPolygonOffsetClampEXT( factor, units, clamp )
%
% C function:  void glPolygonOffsetClampEXT(GLfloat factor, GLfloat units, GLfloat clamp)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glPolygonOffsetClampEXT', factor, units, clamp );

return
