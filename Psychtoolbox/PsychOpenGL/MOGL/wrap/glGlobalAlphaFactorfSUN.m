function glGlobalAlphaFactorfSUN( factor )

% glGlobalAlphaFactorfSUN  Interface to OpenGL function glGlobalAlphaFactorfSUN
%
% usage:  glGlobalAlphaFactorfSUN( factor )
%
% C function:  void glGlobalAlphaFactorfSUN(GLfloat factor)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glGlobalAlphaFactorfSUN', factor );

return
