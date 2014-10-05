function glHistogramEXT( target, width, internalformat, sink )

% glHistogramEXT  Interface to OpenGL function glHistogramEXT
%
% usage:  glHistogramEXT( target, width, internalformat, sink )
%
% C function:  void glHistogramEXT(GLenum target, GLsizei width, GLenum internalformat, GLboolean sink)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glHistogramEXT', target, width, internalformat, sink );

return
