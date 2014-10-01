function r = glGenVertexShadersEXT( range )

% glGenVertexShadersEXT  Interface to OpenGL function glGenVertexShadersEXT
%
% usage:  r = glGenVertexShadersEXT( range )
%
% C function:  GLuint glGenVertexShadersEXT(GLuint range)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

r = moglcore( 'glGenVertexShadersEXT', range );

return
