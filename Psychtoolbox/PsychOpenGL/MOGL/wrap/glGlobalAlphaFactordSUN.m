function glGlobalAlphaFactordSUN( factor )

% glGlobalAlphaFactordSUN  Interface to OpenGL function glGlobalAlphaFactordSUN
%
% usage:  glGlobalAlphaFactordSUN( factor )
%
% C function:  void glGlobalAlphaFactordSUN(GLdouble factor)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glGlobalAlphaFactordSUN', factor );

return
