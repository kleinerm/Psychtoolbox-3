function glTexFilterFuncSGIS( target, filter, n, weights )

% glTexFilterFuncSGIS  Interface to OpenGL function glTexFilterFuncSGIS
%
% usage:  glTexFilterFuncSGIS( target, filter, n, weights )
%
% C function:  void glTexFilterFuncSGIS(GLenum target, GLenum filter, GLsizei n, const GLfloat* weights)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glTexFilterFuncSGIS', target, filter, n, single(weights) );

return
