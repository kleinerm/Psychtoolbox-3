function Cb = AnsiZ136MPEComputeCb(stimulusWavelengthNm)
% Cb = AnsiZ136MPEComputeCb(stimulusWavelengthNm)
%
% Compute constant Cb, ANSI Z136.1-2007, Table 6, p. 76.
%
% This is only defined between 400 and 600 nm.
%
% 2/20/13  dhb  Wrote it.

%% Implement formula.  Factor only defined between 400 and 600 nm
if (stimulusWavelengthNm >= 400 & stimulusWavelengthNm < 450)
    Cb = 1;
elseif (stimulusWavelengthNm >= 450 & stimulusWavelengthNm < 600)
    Cb = 10^(20*(stimulusWavelengthNm/1000-0.450));
else
    Cb = NaN;
end

end


