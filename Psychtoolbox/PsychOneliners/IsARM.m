function resultFlag = IsARM
% resultFlag = IsARM
%
% Returns true if the processor architecture is ARM.

% HISTORY
% 04.04.2013 mk   Wrote it.
% 16.12.2023 mk   Simplify and make more robust and recognize Apple Silicon. 

persistent rc;
if isempty(rc)
     rc = ~isempty(strfind(computer, 'arm-')) || ~isempty(strfind(computer, 'aarch')) || streq(computer, 'MACA64'); %#ok<STREMP>
end

resultFlag = rc;
