function glPathCoordsNV( path, numCoords, coordType, coords )

% glPathCoordsNV  Interface to OpenGL function glPathCoordsNV
%
% usage:  glPathCoordsNV( path, numCoords, coordType, coords )
%
% C function:  void glPathCoordsNV(GLuint path, GLsizei numCoords, GLenum coordType, const void* coords)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glPathCoordsNV', path, numCoords, coordType, coords );

return
