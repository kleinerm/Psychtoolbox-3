function glTransformFeedbackAttribsNV( count, attribs, bufferMode )

% glTransformFeedbackAttribsNV  Interface to OpenGL function glTransformFeedbackAttribsNV
%
% usage:  glTransformFeedbackAttribsNV( count, attribs, bufferMode )
%
% C function:  void glTransformFeedbackAttribsNV(GLsizei count, const GLint* attribs, GLenum bufferMode)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glTransformFeedbackAttribsNV', count, int32(attribs), bufferMode );

return
