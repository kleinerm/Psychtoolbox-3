function glTessellationFactorAMD( factor )

% glTessellationFactorAMD  Interface to OpenGL function glTessellationFactorAMD
%
% usage:  glTessellationFactorAMD( factor )
%
% C function:  void glTessellationFactorAMD(GLfloat factor)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glTessellationFactorAMD', factor );

return
