function glFinalCombinerInputNV( variable, input, mapping, componentUsage )

% glFinalCombinerInputNV  Interface to OpenGL function glFinalCombinerInputNV
%
% usage:  glFinalCombinerInputNV( variable, input, mapping, componentUsage )
%
% C function:  void glFinalCombinerInputNV(GLenum variable, GLenum input, GLenum mapping, GLenum componentUsage)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glFinalCombinerInputNV', variable, input, mapping, componentUsage );

return
