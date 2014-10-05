function glBeginTransformFeedbackNV( primitiveMode )

% glBeginTransformFeedbackNV  Interface to OpenGL function glBeginTransformFeedbackNV
%
% usage:  glBeginTransformFeedbackNV( primitiveMode )
%
% C function:  void glBeginTransformFeedbackNV(GLenum primitiveMode)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glBeginTransformFeedbackNV', primitiveMode );

return
