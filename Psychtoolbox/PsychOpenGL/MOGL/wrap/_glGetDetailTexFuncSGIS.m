function points = glGetDetailTexFuncSGIS( target )

% glGetDetailTexFuncSGIS  Interface to OpenGL function glGetDetailTexFuncSGIS
%
% usage:  points = glGetDetailTexFuncSGIS( target )
%
% C function:  void glGetDetailTexFuncSGIS(GLenum target, GLfloat* points)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=1,
    error('invalid number of arguments');
end

points = single(0);

moglcore( 'glGetDetailTexFuncSGIS', target, points );

return
