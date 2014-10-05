function glTessellationModeAMD( mode )

% glTessellationModeAMD  Interface to OpenGL function glTessellationModeAMD
%
% usage:  glTessellationModeAMD( mode )
%
% C function:  void glTessellationModeAMD(GLenum mode)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glTessellationModeAMD', mode );

return
