function glPathStencilDepthOffsetNV( factor, units )

% glPathStencilDepthOffsetNV  Interface to OpenGL function glPathStencilDepthOffsetNV
%
% usage:  glPathStencilDepthOffsetNV( factor, units )
%
% C function:  void glPathStencilDepthOffsetNV(GLfloat factor, GLfloat units)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glPathStencilDepthOffsetNV', factor, units );

return
