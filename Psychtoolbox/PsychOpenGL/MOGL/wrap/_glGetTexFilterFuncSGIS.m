function weights = glGetTexFilterFuncSGIS( target, filter )

% glGetTexFilterFuncSGIS  Interface to OpenGL function glGetTexFilterFuncSGIS
%
% usage:  weights = glGetTexFilterFuncSGIS( target, filter )
%
% C function:  void glGetTexFilterFuncSGIS(GLenum target, GLenum filter, GLfloat* weights)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

weights = single(0);

moglcore( 'glGetTexFilterFuncSGIS', target, filter, weights );

return
