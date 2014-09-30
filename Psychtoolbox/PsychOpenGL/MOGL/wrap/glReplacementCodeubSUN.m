function glReplacementCodeubSUN( code )

% glReplacementCodeubSUN  Interface to OpenGL function glReplacementCodeubSUN
%
% usage:  glReplacementCodeubSUN( code )
%
% C function:  void glReplacementCodeubSUN(GLubyte code)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glReplacementCodeubSUN', code );

return
