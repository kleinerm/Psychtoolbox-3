function glGlobalAlphaFactorusSUN( factor )

% glGlobalAlphaFactorusSUN  Interface to OpenGL function glGlobalAlphaFactorusSUN
%
% usage:  glGlobalAlphaFactorusSUN( factor )
%
% C function:  void glGlobalAlphaFactorusSUN(GLushort factor)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glGlobalAlphaFactorusSUN', factor );

return
