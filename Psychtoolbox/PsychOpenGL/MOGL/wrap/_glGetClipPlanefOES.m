function equation = glGetClipPlanefOES( plane )

% glGetClipPlanefOES  Interface to OpenGL function glGetClipPlanefOES
%
% usage:  equation = glGetClipPlanefOES( plane )
%
% C function:  void glGetClipPlanefOES(GLenum plane, GLfloat* equation)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=1,
    error('invalid number of arguments');
end

equation = single(0);

moglcore( 'glGetClipPlanefOES', plane, equation );

return
