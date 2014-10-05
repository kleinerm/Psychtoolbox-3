function glPathCommandsNV( path, numCommands, commands, numCoords, coordType, coords )

% glPathCommandsNV  Interface to OpenGL function glPathCommandsNV
%
% usage:  glPathCommandsNV( path, numCommands, commands, numCoords, coordType, coords )
%
% C function:  void glPathCommandsNV(GLuint path, GLsizei numCommands, const GLubyte* commands, GLsizei numCoords, GLenum coordType, const void* coords)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=6,
    error('invalid number of arguments');
end

moglcore( 'glPathCommandsNV', path, numCommands, uint8(commands), numCoords, coordType, coords );

return
