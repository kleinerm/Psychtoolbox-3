function r = glGenFragmentShadersATI( range )

% glGenFragmentShadersATI  Interface to OpenGL function glGenFragmentShadersATI
%
% usage:  r = glGenFragmentShadersATI( range )
%
% C function:  GLuint glGenFragmentShadersATI(GLuint range)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

r = moglcore( 'glGenFragmentShadersATI', range );

return
