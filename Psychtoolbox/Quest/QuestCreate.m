function q=QuestCreate(tGuess,tGuessSd,pThreshold,beta,delta,gamma,grain,range,plotIt)
% q=QuestCreate(tGuess,tGuessSd,pThreshold,beta,delta,gamma,[grain],[range],[plotIt])
%
% Create a struct q with all the information necessary to measure
% threshold. Threshold "t" is measured on an abstract "intensity"
% scale, which usually corresponds to log10 contrast.
% 
% QuestCreate saves in struct q the parameters for a Weibull psychometric function:
% p2=delta*gamma+(1-delta)*(1-(1-gamma)*exp(-10.^(beta*(x-xThreshold))));
% where x represents log10 contrast relative to threshold. The Weibull
% function itself appears only in QuestRecompute, which uses the
% specified parameter values in q to compute a psychometric function
% and store it in q. All the other Quest functions simply use the
% psychometric function stored in "q". QuestRecompute is called solely
% by QuestCreate and QuestBetaAnalysis (and possibly by a few user
% programs). Thus, if you prefer to use a different kind of
% psychometric function, called Foo, you need only create your own
% QuestCreateFoo, QuestRecomputeFoo, and (if you need it)
% QuestBetaAnalysisFoo, based on QuestCreate, QuestRecompute, and
% QuestBetaAnalysis, and you can use them with the rest of the Quest
% package unchanged. You would only be changing a few lines of code,
% so it would quite easy to do.
% 
% Several users of Quest have asked questions on the Psychtoolbox forum
% about how to restrict themselves to a practical testing range. That is
% not what tGuessSd and "range" are for; they should be large, e.g. I
% typically set tGuessSd=3 and range=5 when intensity represents log
% contrast. If necessary, you should restrict the range yourself, outside
% of Quest. Here, in QuestCreate, you tell Quest about your prior beliefs,
% and you should try to be open-minded, giving Quest a generously large
% range to consider as possible values of threshold. For each trial you
% will later ask Quest to suggest a test intensity. It is important to
% realize that what Quest returns is just what you asked for, a
% suggestion. You should then test at whatever intensity you like, taking
% into account both the suggestion and any practical constraints (e.g. a
% maximum and minimum contrast that you can achieve, and quantization of
% contrast). After running the trial you should call QuestUpdate with the
% contrast that you actually used and the observer's response to add your
% new datum to the database. Don't restrict "tGuessSd" or "range" by the
% limitations of what you can display. Keep open the possibility that
% threshold may lie outside the range of contrasts that you can produce,
% and let Quest consider all possibilities.
% 
% There is one exception to the above advice of always being generous with
% tGuessSd. Occasionally we find that we have a working Quest-based
% program that measures threshold, and we discover that we need to measure
% the proportion correct at a particular intensity. Instead of writing a
% new program, or modifying the old one, it is often more convenient to
% instead reduce tGuessSd to practically zero, e.g. a value like 0.001,
% which has the effect of restricting all threshold estimates to be
% practically identical to tGuess, making it easy to run any number of
% trials at that intensity. Of course, in this case, the final threshold
% estimate from Quest should be ignored, since it is merely parroting back
% to you the assertion that threshold is equal to the initial guess
% "tGuess". What's of interest is the final proportion correct; at the
% end, call QuestTrials or add an FPRINTF statement to report it.
% 
% tGuess is your prior threshold estimate.
% tGuessSd is the standard deviation you assign to that guess. Be generous. 
% pThreshold is your threshold criterion expressed as probability of 
%	response==1. An intensity offset is introduced into the psychometric 
%	function so that threshold (i.e. the midpoint of the table) yields 
%	pThreshold.
% beta, delta, and gamma are the parameters of a Weibull psychometric function.
% beta controls the steepness of the psychometric function. Typically 3.5.
% delta is the fraction of trials on which the observer presses blindly. 
%	Typically 0.01.
% gamma is the fraction of trials that will generate response 1 when 
%	intensity==-inf.
% grain is the quantization (step size) of the internal table. E.g. 0.01.
% range is the intensity difference between the largest and smallest
% 	intensity that the internal table can store. E.g. 5. This interval will
% 	be centered on the initial guess tGuess, i.e.
% 	tGuess+(-range/2:grain:range/2). "range" is used only momentarily here,
% 	to determine "dim", which is retained in the quest struct. "dim" is the
% 	number of distinct intensities that the internal table can store, e.g.
% 	500. QUEST assumes that intensities outside of this interval have zero
% 	prior probability, i.e. they are impossible values for threshold. The
% 	cost of making "range" too big is some extra storage and computation,
% 	which are usually negligible. The cost of making "range" too small is
% 	that you prejudicially exclude what are actually possible values for
% 	threshold. Getting out-of-range warnings from QuestUpdate is one
% 	possible indication that your stated range is too small.
% 
% See Quest.

% 6/8/96   dgp  Wrote it.
% 6/11/96  dgp  Optimized the order of stuffing for faster unstuffing.
% 11/10/96 dhb  Added warning about correctness after DGP told me.
% 3/1/97   dgp  Fixed error in sign of xThreshold in formula for p2.
% 3/1/97   dgp  Updated to use Matlab 5 structs.
% 3/3/97   dhb  Added missing semicolon to first struct eval.
% 3/5/97   dgp  Fixed sd: use exp instead of 10^.
% 3/5/97   dgp  Added some explanation of the psychometric function.
% 6/24/97   dgp  For simulations, now allow specification of grain and dim.
% 9/30/98	dgp	Added "dim" fix from Richard Murray.
% 4/12/99 dgp dropped support for Matlab 4.
% 5/6/99 dgp Simplified "dim" calculation; just round up to even integer.
% 8/15/99   dgp  Explain how to use other kind of psychometric function.
% 2/10/02   dgp  Document grain and range.
% 9/11/04   dgp  Explain why supplied "range" should err on the high side.
% 10/13/04 	dgp  Explain why tGuesSd and range should be large, generous. 
% 10/13/04 	dgp  Set q.normalizePdf to 1, to avoid underflow errors that otherwise accur after around 1000 trials.
% 8/23/15  dgp Flag error if tGuess is not finite.
% 
% Copyright (c) 1996-2004 Denis Pelli
if nargin < 6 || nargin > 9
	error('Usage: q=QuestCreate(tGuess,tGuessSd,pThreshold,beta,delta,gamma,[grain],[range])')
end

if nargin < 7 || isempty(grain)
	grain=0.01;
end

if nargin < 8 || isempty(range)
	dim=500;
else
	if range<=0
		error('"range" must be greater than zero.')
	end
	dim=range/grain;
	dim=2*ceil(dim/2);	% round up to an even integer
end

if nargin < 9 || isempty(plotIt)
    plotIt = 0;
end

if ~isfinite(tGuess) || ~isreal(tGuess)
    error('"tGuess" must be real and finite.');
end

q.updatePdf=1; % boolean: 0 for no, 1 for yes
q.warnPdf=1; % boolean
q.normalizePdf=1; % boolean. This adds a few ms per call to QuestUpdate, but otherwise the pdf will underflow after about 1000 trials.
q.tGuess=tGuess;
q.tGuessSd=tGuessSd;
q.pThreshold=pThreshold;
q.beta=beta;
q.delta=delta;
q.gamma=gamma;
q.grain=grain;
q.dim=dim;
q=QuestRecompute(q, plotIt);

% THIS CODE WAS IN THE OLD VERSION. I'VE PASTED "q." INTO THE OBVIOUS PLACES.
% THIS IS RETAINED SOLELY TO HELP DEBUG ANY BUGS IN THE NEW CODE.
% % prepare all the arrays
% q.i=-dim/2:dim/2;
% q.x=i*grain;
% q.pdf=exp(-0.5*(q.x/tGuessSd).^2);
% q.pdf=q.pdf/sum(q.pdf);			% normalize the pdf 
% i2=-dim:dim;
% q.x2=i2*q.grain;
% q.p2=delta*gamma+(1-delta)*(1-(1-gamma)*exp(-10.^(beta*q.x2)));
% index=find(diff(q.p2)); 		% subset that is strictly monotonic
% q.xThreshold=interp1(q.p2(index),q.x2(index),q.pThreshold);
% q.p2=delta*gamma+(1-delta)*(1-(1-gamma)*exp(-10.^(beta*(q.x2+q.xThreshold))));
% q.s2=fliplr([1-q.p2;q.p2]);
% 
% % Best quantileOrder depends only on min and max of psychometric function.
% % For 2-interval forced choice, if pL=0.5 and pH=1 then best quantileOrder=0.60
% % We write x*log(x+eps) in place of x*log(x) to get zero instead of NAN when x is zero.
% pL=q.p2(1);
% pH=q.p2(end);
% pE=pH*log(pH+eps)-pL*log(pL+eps)+(1-pH+eps)*log(1-pH+eps)-(1-pL+eps)*log(1-pL+eps);
% pE=1/(1+exp(pE/(pL-pH)));
% q.quantileOrder=(pE-pL)/(pH-pL);
