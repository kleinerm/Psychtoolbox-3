function glGlobalAlphaFactorsSUN( factor )

% glGlobalAlphaFactorsSUN  Interface to OpenGL function glGlobalAlphaFactorsSUN
%
% usage:  glGlobalAlphaFactorsSUN( factor )
%
% C function:  void glGlobalAlphaFactorsSUN(GLshort factor)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glGlobalAlphaFactorsSUN', factor );

return
