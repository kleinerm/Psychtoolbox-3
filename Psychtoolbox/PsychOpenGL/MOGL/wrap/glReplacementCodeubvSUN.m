function glReplacementCodeubvSUN( code )

% glReplacementCodeubvSUN  Interface to OpenGL function glReplacementCodeubvSUN
%
% usage:  glReplacementCodeubvSUN( code )
%
% C function:  void glReplacementCodeubvSUN(const GLubyte* code)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glReplacementCodeubvSUN', uint8(code) );

return
