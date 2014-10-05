function glFogFuncSGIS( n, points )

% glFogFuncSGIS  Interface to OpenGL function glFogFuncSGIS
%
% usage:  glFogFuncSGIS( n, points )
%
% C function:  void glFogFuncSGIS(GLsizei n, const GLfloat* points)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glFogFuncSGIS', n, single(points) );

return
