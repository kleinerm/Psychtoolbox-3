function glReferencePlaneSGIX( equation )

% glReferencePlaneSGIX  Interface to OpenGL function glReferencePlaneSGIX
%
% usage:  glReferencePlaneSGIX( equation )
%
% C function:  void glReferencePlaneSGIX(const GLdouble* equation)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glReferencePlaneSGIX', double(equation) );

return
