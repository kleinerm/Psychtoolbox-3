function glGlobalAlphaFactorubSUN( factor )

% glGlobalAlphaFactorubSUN  Interface to OpenGL function glGlobalAlphaFactorubSUN
%
% usage:  glGlobalAlphaFactorubSUN( factor )
%
% C function:  void glGlobalAlphaFactorubSUN(GLubyte factor)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glGlobalAlphaFactorubSUN', factor );

return
