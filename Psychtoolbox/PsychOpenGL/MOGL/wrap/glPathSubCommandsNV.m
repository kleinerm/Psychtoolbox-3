function glPathSubCommandsNV( path, commandStart, commandsToDelete, numCommands, commands, numCoords, coordType, coords )

% glPathSubCommandsNV  Interface to OpenGL function glPathSubCommandsNV
%
% usage:  glPathSubCommandsNV( path, commandStart, commandsToDelete, numCommands, commands, numCoords, coordType, coords )
%
% C function:  void glPathSubCommandsNV(GLuint path, GLsizei commandStart, GLsizei commandsToDelete, GLsizei numCommands, const GLubyte* commands, GLsizei numCoords, GLenum coordType, const void* coords)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=8,
    error('invalid number of arguments');
end

moglcore( 'glPathSubCommandsNV', path, commandStart, commandsToDelete, numCommands, uint8(commands), numCoords, coordType, coords );

return
