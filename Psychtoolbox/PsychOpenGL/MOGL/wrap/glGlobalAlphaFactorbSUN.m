function glGlobalAlphaFactorbSUN( factor )

% glGlobalAlphaFactorbSUN  Interface to OpenGL function glGlobalAlphaFactorbSUN
%
% usage:  glGlobalAlphaFactorbSUN( factor )
%
% C function:  void glGlobalAlphaFactorbSUN(GLbyte factor)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glGlobalAlphaFactorbSUN', factor );

return
