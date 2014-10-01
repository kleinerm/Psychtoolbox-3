function glClipPlanefOES( plane, equation )

% glClipPlanefOES  Interface to OpenGL function glClipPlanefOES
%
% usage:  glClipPlanefOES( plane, equation )
%
% C function:  void glClipPlanefOES(GLenum plane, const GLfloat* equation)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glClipPlanefOES', plane, single(equation) );

return
