function values = glGetHistogramEXT( target, reset, format, type )

% glGetHistogramEXT  Interface to OpenGL function glGetHistogramEXT
%
% usage:  values = glGetHistogramEXT( target, reset, format, type )
%
% C function:  void glGetHistogramEXT(GLenum target, GLboolean reset, GLenum format, GLenum type, void* values)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=4,
    error('invalid number of arguments');
end

values = (0);

moglcore( 'glGetHistogramEXT', target, reset, format, type, values );

return
