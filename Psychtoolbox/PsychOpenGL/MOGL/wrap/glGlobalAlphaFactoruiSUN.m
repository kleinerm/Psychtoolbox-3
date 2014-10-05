function glGlobalAlphaFactoruiSUN( factor )

% glGlobalAlphaFactoruiSUN  Interface to OpenGL function glGlobalAlphaFactoruiSUN
%
% usage:  glGlobalAlphaFactoruiSUN( factor )
%
% C function:  void glGlobalAlphaFactoruiSUN(GLuint factor)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glGlobalAlphaFactoruiSUN', factor );

return
