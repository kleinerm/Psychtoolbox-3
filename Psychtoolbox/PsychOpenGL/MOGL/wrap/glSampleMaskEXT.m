function glSampleMaskEXT( value, invert )

% glSampleMaskEXT  Interface to OpenGL function glSampleMaskEXT
%
% usage:  glSampleMaskEXT( value, invert )
%
% C function:  void glSampleMaskEXT(GLclampf value, GLboolean invert)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glSampleMaskEXT', value, invert );

return
