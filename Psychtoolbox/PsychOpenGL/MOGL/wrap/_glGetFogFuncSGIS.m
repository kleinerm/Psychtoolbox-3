function points = glGetFogFuncSGIS

% glGetFogFuncSGIS  Interface to OpenGL function glGetFogFuncSGIS
%
% usage:  points = glGetFogFuncSGIS
%
% C function:  void glGetFogFuncSGIS(GLfloat* points)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=0,
    error('invalid number of arguments');
end

points = single(0);

moglcore( 'glGetFogFuncSGIS', points );

return
