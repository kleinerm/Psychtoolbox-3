function glSharpenTexFuncSGIS( target, n, points )

% glSharpenTexFuncSGIS  Interface to OpenGL function glSharpenTexFuncSGIS
%
% usage:  glSharpenTexFuncSGIS( target, n, points )
%
% C function:  void glSharpenTexFuncSGIS(GLenum target, GLsizei n, const GLfloat* points)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glSharpenTexFuncSGIS', target, n, single(points) );

return
