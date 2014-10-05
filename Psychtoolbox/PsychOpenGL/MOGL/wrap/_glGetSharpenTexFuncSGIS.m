function points = glGetSharpenTexFuncSGIS( target )

% glGetSharpenTexFuncSGIS  Interface to OpenGL function glGetSharpenTexFuncSGIS
%
% usage:  points = glGetSharpenTexFuncSGIS( target )
%
% C function:  void glGetSharpenTexFuncSGIS(GLenum target, GLfloat* points)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=1,
    error('invalid number of arguments');
end

points = single(0);

moglcore( 'glGetSharpenTexFuncSGIS', target, points );

return
