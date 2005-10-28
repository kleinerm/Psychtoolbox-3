function epsilon = FindEpsWeibTAFC(pCorrect,beta)
% epsilon = FindEpsWeibTAFC(pCorrect,beta)
%
% Compute the epsilon corresponding to a particular
% percent correct.  Mainly used with quest.
%
% Note that this is a less flexible version of the
% computation performed by QUEST('Epsilon').  This
% routine does not allow variations in gamma, delta
% or the psychometric function.  It agrees with
% what QUEST('Epsilon') computes for gamma = 0.5,
% delta = 0, Weibull function.
%
% It would not be hard to fix this to include delta
% and gamma as parameters, but QUEST('Epsilon') seems
% to do the trick.
%
% 8/26/94		ccc, dhb		Wrote it.
% 8/30/94		dhb					Comment comparing to QUEST('Epsilon').

% Set gamma value for TAFC
gamma = 0.5;

if (pCorrect == 1)
	pCorrect = 1-1e-15;
end

factor1 = - log( (1-pCorrect)/(1-gamma) );
epsilon = (1/beta)*log10(factor1);
