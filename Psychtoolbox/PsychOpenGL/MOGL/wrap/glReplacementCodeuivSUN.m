function glReplacementCodeuivSUN( code )

% glReplacementCodeuivSUN  Interface to OpenGL function glReplacementCodeuivSUN
%
% usage:  glReplacementCodeuivSUN( code )
%
% C function:  void glReplacementCodeuivSUN(const GLuint* code)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glReplacementCodeuivSUN', uint32(code) );

return
