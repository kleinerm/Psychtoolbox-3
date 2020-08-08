function glViewportPositionWScaleNV( index, xcoeff, ycoeff )

% glViewportPositionWScaleNV  Interface to OpenGL function glViewportPositionWScaleNV
%
% usage:  glViewportPositionWScaleNV( index, xcoeff, ycoeff )
%
% C function:  void glViewportPositionWScaleNV(GLuint index, GLfloat xcoeff, GLfloat ycoeff)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glViewportPositionWScaleNV', index, xcoeff, ycoeff );

return
