function glBeginTransformFeedbackEXT( primitiveMode )

% glBeginTransformFeedbackEXT  Interface to OpenGL function glBeginTransformFeedbackEXT
%
% usage:  glBeginTransformFeedbackEXT( primitiveMode )
%
% C function:  void glBeginTransformFeedbackEXT(GLenum primitiveMode)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glBeginTransformFeedbackEXT', primitiveMode );

return
