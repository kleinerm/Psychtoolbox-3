function glBindTransformFeedbackNV( target, id )

% glBindTransformFeedbackNV  Interface to OpenGL function glBindTransformFeedbackNV
%
% usage:  glBindTransformFeedbackNV( target, id )
%
% C function:  void glBindTransformFeedbackNV(GLenum target, GLuint id)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glBindTransformFeedbackNV', target, id );

return
