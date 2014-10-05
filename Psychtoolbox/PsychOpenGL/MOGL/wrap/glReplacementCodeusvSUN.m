function glReplacementCodeusvSUN( code )

% glReplacementCodeusvSUN  Interface to OpenGL function glReplacementCodeusvSUN
%
% usage:  glReplacementCodeusvSUN( code )
%
% C function:  void glReplacementCodeusvSUN(const GLushort* code)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glReplacementCodeusvSUN', uint16(code) );

return
