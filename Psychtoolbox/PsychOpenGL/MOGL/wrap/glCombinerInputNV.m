function glCombinerInputNV( stage, portion, variable, input, mapping, componentUsage )

% glCombinerInputNV  Interface to OpenGL function glCombinerInputNV
%
% usage:  glCombinerInputNV( stage, portion, variable, input, mapping, componentUsage )
%
% C function:  void glCombinerInputNV(GLenum stage, GLenum portion, GLenum variable, GLenum input, GLenum mapping, GLenum componentUsage)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=6,
    error('invalid number of arguments');
end

moglcore( 'glCombinerInputNV', stage, portion, variable, input, mapping, componentUsage );

return
