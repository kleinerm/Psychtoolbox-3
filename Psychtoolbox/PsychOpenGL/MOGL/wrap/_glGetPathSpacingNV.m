function returnedSpacing = glGetPathSpacingNV( pathListMode, numPaths, pathNameType, paths, pathBase, advanceScale, kerningScale, transformType )

% glGetPathSpacingNV  Interface to OpenGL function glGetPathSpacingNV
%
% usage:  returnedSpacing = glGetPathSpacingNV( pathListMode, numPaths, pathNameType, paths, pathBase, advanceScale, kerningScale, transformType )
%
% C function:  void glGetPathSpacingNV(GLenum pathListMode, GLsizei numPaths, GLenum pathNameType, const void* paths, GLuint pathBase, GLfloat advanceScale, GLfloat kerningScale, GLenum transformType, GLfloat* returnedSpacing)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=8,
    error('invalid number of arguments');
end

returnedSpacing = single(0);

moglcore( 'glGetPathSpacingNV', pathListMode, numPaths, pathNameType, paths, pathBase, advanceScale, kerningScale, transformType, returnedSpacing );

return
