function r = glGenPathsNV( range )

% glGenPathsNV  Interface to OpenGL function glGenPathsNV
%
% usage:  r = glGenPathsNV( range )
%
% C function:  GLuint glGenPathsNV(GLsizei range)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

r = moglcore( 'glGenPathsNV', range );

return
