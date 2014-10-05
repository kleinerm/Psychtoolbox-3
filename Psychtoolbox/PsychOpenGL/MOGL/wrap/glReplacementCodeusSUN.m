function glReplacementCodeusSUN( code )

% glReplacementCodeusSUN  Interface to OpenGL function glReplacementCodeusSUN
%
% usage:  glReplacementCodeusSUN( code )
%
% C function:  void glReplacementCodeusSUN(GLushort code)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glReplacementCodeusSUN', code );

return
