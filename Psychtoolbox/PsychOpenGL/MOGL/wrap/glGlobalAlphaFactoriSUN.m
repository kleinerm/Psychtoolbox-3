function glGlobalAlphaFactoriSUN( factor )

% glGlobalAlphaFactoriSUN  Interface to OpenGL function glGlobalAlphaFactoriSUN
%
% usage:  glGlobalAlphaFactoriSUN( factor )
%
% C function:  void glGlobalAlphaFactoriSUN(GLint factor)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glGlobalAlphaFactoriSUN', factor );

return
