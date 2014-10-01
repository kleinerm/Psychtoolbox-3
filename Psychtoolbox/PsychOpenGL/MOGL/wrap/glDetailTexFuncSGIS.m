function glDetailTexFuncSGIS( target, n, points )

% glDetailTexFuncSGIS  Interface to OpenGL function glDetailTexFuncSGIS
%
% usage:  glDetailTexFuncSGIS( target, n, points )
%
% C function:  void glDetailTexFuncSGIS(GLenum target, GLsizei n, const GLfloat* points)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glDetailTexFuncSGIS', target, n, single(points) );

return
