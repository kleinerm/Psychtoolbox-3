function queryId = glGetFirstPerfQueryIdINTEL

% glGetFirstPerfQueryIdINTEL  Interface to OpenGL function glGetFirstPerfQueryIdINTEL
%
% usage:  queryId = glGetFirstPerfQueryIdINTEL
%
% C function:  void glGetFirstPerfQueryIdINTEL(GLuint* queryId)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=0,
    error('invalid number of arguments');
end

queryId = uint32(0);

moglcore( 'glGetFirstPerfQueryIdINTEL', queryId );

return
