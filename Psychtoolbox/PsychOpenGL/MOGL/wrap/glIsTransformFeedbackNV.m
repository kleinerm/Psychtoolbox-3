function r = glIsTransformFeedbackNV( id )

% glIsTransformFeedbackNV  Interface to OpenGL function glIsTransformFeedbackNV
%
% usage:  r = glIsTransformFeedbackNV( id )
%
% C function:  GLboolean glIsTransformFeedbackNV(GLuint id)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

r = moglcore( 'glIsTransformFeedbackNV', id );

return
