function glMinSampleShading( value )

% glMinSampleShading  Interface to OpenGL function glMinSampleShading
%
% usage:  glMinSampleShading( value )
%
% C function:  void glMinSampleShading(GLfloat value)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glMinSampleShading', value );

return
