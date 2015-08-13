function q=QuestUpdate(q,intensity,response)
% q=QuestUpdate(q,intensity,response)
%
% Update the struct q to reflect the results of this trial. The historical
% records q.intensity and q.response are always updated, but q.pdf is only
% updated if q.updatePdf is true. You can always call QuestRecompute to
% recreate q.pdf from scratch from the historical record.
% 
% See Quest.

% Denis Pelli, 6/11/96
% 2/28/97 dgp Updated for Matlab 5: call round.
% 4/12/99 dgp Dropped support for Matlab 4.
% 4/30/99 dgp Give explanatory error message when intensity is out of bounds.
% 9/11/04 dgp For compatibility with Matlab 6.5, comment out the testing
%             of WARNING level.
%
% Copyright (c) 1996-2004 Denis Pelli
%
% 10/31/10 mk Allocate q.intensity and q.response in chunks of 10000
%             trials to reduce memory fragmentation problems.
% 8/1/15 dgp  Make sure "intensity" is real, not complex. Complex values
%             were getting a less-than-clear warning. Now it's an error.
%             This can happen easily in the typical use of log10(value) as
%             the "intensity" for Quest. If value is negative the intensity
%             will be complex. Now you'll get an explicit error alerting
%             you that this happened.

if nargin~=3
	error('Usage: q=QuestUpdate(q,intensity,response)')
end
if length(q)>1
	error('Can''t deal with q being a vector.')
end
if ~isreal(intensity)
    error(sprintf('QuestUpdate: intensity %s must be real, not complex.',num2str(intensity)));
end
if response<0 || response>=size(q.s2,1)
	error(sprintf('response %g out of range 0 to %d',response,size(q.s2,1)-1))
end
if q.updatePdf
	inten=max(-1e10,min(1e10,intensity)); % make intensity finite
	ii=size(q.pdf,2)+q.i-round((inten-q.tGuess)/q.grain);
	if ii(1)<1 || ii(end)>size(q.s2,2)
		if q.warnPdf
			low=(1-size(q.pdf,2)-q.i(1))*q.grain+q.tGuess;
			high=(size(q.s2,2)-size(q.pdf,2)-q.i(end))*q.grain+q.tGuess;
			oldWarning=warning;
			warning('on'); %#ok<WNON> % no backtrace
			warning(sprintf('QuestUpdate: intensity %.3f out of range %.2f to %.2f. Pdf will be inexact. Suggest that you increase "range" in call to QuestCreate.',intensity,low,high)); %#ok<SPWRN>
			warning(oldWarning);
		end
		if ii(1)<1
			ii=ii+1-ii(1);
		else
			ii=ii+size(q.s2,2)-ii(end);
		end
	end
	q.pdf=q.pdf.*q.s2(response+1,ii); % 4 ms
	if q.normalizePdf
		q.pdf=q.pdf/sum(q.pdf);		% keep the pdf normalized	% 3 ms
	end
end

% keep a historical record of the trials
q.trialCount = q.trialCount + 1;
if q.trialCount > length(q.intensity)
    % Out of space in preallocated arrays. Reallocate for additional
    % 10000 trials. We reallocate in large chunks to reduce memory
    % fragmentation.
    q.intensity = [q.intensity, zeros(1,10000)];
    q.response  = [q.response,  zeros(1,10000)];
end

q.intensity(q.trialCount) = intensity;
q.response(q.trialCount)  = response;
