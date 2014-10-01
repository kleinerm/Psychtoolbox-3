function nextQueryId = glGetNextPerfQueryIdINTEL( queryId )

% glGetNextPerfQueryIdINTEL  Interface to OpenGL function glGetNextPerfQueryIdINTEL
%
% usage:  nextQueryId = glGetNextPerfQueryIdINTEL( queryId )
%
% C function:  void glGetNextPerfQueryIdINTEL(GLuint queryId, GLuint* nextQueryId)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=1,
    error('invalid number of arguments');
end

nextQueryId = uint32(0);

moglcore( 'glGetNextPerfQueryIdINTEL', queryId, nextQueryId );

return
