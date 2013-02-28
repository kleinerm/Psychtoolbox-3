function Ce = AnsiZ136MPEComputeCe(stimulusSizeDeg)
% Ce = AnsiZ136MPEComputeCe(stimulusSizeDeg)
%
% Compute constant Ce, ANSI Z136.1-2007, Table 6, p. 76.
%
% This is only defined between 400 and 1400 nm.
%
% 2/20/13  dhb  Wrote it.

%% Implement formula.  Factor only defined between 400 and 600 nm
stimulusSizeMrad = DegToMrad(stimulusSizeDeg);
if (stimulusSizeMrad < 1.5)
    Ce = 1;
elseif (stimulusSizeMrad > 100)
    Ce = (stimulusSizeMrad^2)/(1.5*100);
else
    Ce = stimulusSizeMrad/1.5;
end

end


