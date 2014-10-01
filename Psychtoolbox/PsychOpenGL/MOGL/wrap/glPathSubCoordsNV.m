function glPathSubCoordsNV( path, coordStart, numCoords, coordType, coords )

% glPathSubCoordsNV  Interface to OpenGL function glPathSubCoordsNV
%
% usage:  glPathSubCoordsNV( path, coordStart, numCoords, coordType, coords )
%
% C function:  void glPathSubCoordsNV(GLuint path, GLsizei coordStart, GLsizei numCoords, GLenum coordType, const void* coords)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glPathSubCoordsNV', path, coordStart, numCoords, coordType, coords );

return
