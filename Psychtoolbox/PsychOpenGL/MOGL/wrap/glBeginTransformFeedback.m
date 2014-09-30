function glBeginTransformFeedback( primitiveMode )

% glBeginTransformFeedback  Interface to OpenGL function glBeginTransformFeedback
%
% usage:  glBeginTransformFeedback( primitiveMode )
%
% C function:  void glBeginTransformFeedback(GLenum primitiveMode)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glBeginTransformFeedback', primitiveMode );

return
