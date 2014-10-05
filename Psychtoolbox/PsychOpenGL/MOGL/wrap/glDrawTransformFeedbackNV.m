function glDrawTransformFeedbackNV( mode, id )

% glDrawTransformFeedbackNV  Interface to OpenGL function glDrawTransformFeedbackNV
%
% usage:  glDrawTransformFeedbackNV( mode, id )
%
% C function:  void glDrawTransformFeedbackNV(GLenum mode, GLuint id)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glDrawTransformFeedbackNV', mode, id );

return
