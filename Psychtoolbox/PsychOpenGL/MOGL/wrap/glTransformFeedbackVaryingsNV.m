function glTransformFeedbackVaryingsNV( program, count, locations, bufferMode )

% glTransformFeedbackVaryingsNV  Interface to OpenGL function glTransformFeedbackVaryingsNV
%
% usage:  glTransformFeedbackVaryingsNV( program, count, locations, bufferMode )
%
% C function:  void glTransformFeedbackVaryingsNV(GLuint program, GLsizei count, const GLint* locations, GLenum bufferMode)

% 29-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glTransformFeedbackVaryingsNV', program, count, int32(locations), bufferMode );

return
