% QuestDemo.m
% 
% One of the great contributions of psychophysics to psychology is the
% notion of measuring threshold, i.e. the signal strength required for a
% criterion level of response by the observer (Pelli & Farell, 1994;
% Farell & Pelli, 1999). Watson and Pelli (1983) described a maximum
% likelihood procedure, which they called QUEST, for estimating threshold.
% The Quest toolbox in the Psychtoolbox is a set of MATLAB functions
% that implement all the original QUEST functions, plus several others.
% You can think of it as a Bayesian toolbox for testing observers and
% estimating their thresholds. This QUEST toolbox is self-contained,
% and runs on any computer with MATLAB 5 or better.
% web http://psychtoolbox.org/
% web http://psych.nyu.edu/pelli/software.html#quest
% 
% By commenting and uncommenting five lines below, you can use this file
% to implement three QUEST-related procedures for measuring threshold.
% 
% QuestMode: In the original algorithm of Watson & Pelli (1983), each
% trial is at the MODE of the posterior pdf. Their final estimate is
% maximum likelihood, which is the MODE of the posterior pdf after
% dividing out the prior pdf. (Subsequent experience has shown that it's
% better not to divide out the prior, simply using MODE of posterior pdf
% throughout.)
% 
% QuestMean: In the improved algorithm of King-Smith et al. (1994), each
% trial and the final estimate are at the MEAN of the posterior pdf.
% 
% QuestQuantile & QuestMean: In the ideal algorithm of Pelli (1987), each
% trial is at the best QUANTILE, and the final estimate is at the MEAN of
% the posterior pdf.
% 
% You begin by calling QuestCreate, telling Quest what is your prior
% knowledge, i.e. a guess and associated sd for threshold. Then you run
% some number of trials, typically 40. For each trial you ask Quest to
% recommend a test intensity. Then you actually test the observer at some
% intensity, not necessarily what Quest recommended, and then you call
% QuestUpdate to report to Quest the actual intensity used and whether the
% observer got it right. Quest saves this information in your Quest struct, 
% which we usually call "q". This cycle is repeated for each trial. Finally,
% at the end, when you're done, you ask Quest to provide a final threshold
% estimate, usually the mean and sd (of the posterior pdf).
% 
% It is important to realize that Quest is merely a friendly adviser,
% cataloging your data in your q structure, and making statistical
% analyses of it, but never giving you orders. You're still in charge. On
% each trial, you ask Quest (by calling QuestMode, or QuestMean, or
% QuestQuantile) to suggest the best intensity for the next trial. Taking
% that as advice, in your experiment you should then select the intensity
% yourself for the next trial, taking into account the limitations of your
% equipment and experiment. Typically you'll impose a maximum and a
% minimum, but your equipment may also restrict you to particular discrete
% values, and you might have some reason for not repeating a value.
% Typically you'll choose the available intensity closest to what Quest
% recommended. In some cases the process of producing the stimulus is so
% involved that the exact stimulus intensity is known only after it's been 
% shown. Having run the trial, you then report the new datum,
% the actual intensity tested and the observer's response, asking Quest to
% add it to the database in q.
% 
% To use Quest you must provide an estimated value for beta. Beta
% controls the steepness of the Weibull function. Many vision studies use
% Michelson contrast to control the visibility of the stimulus. It turns
% out that psychometric functions for 2afc detection as a function of
% contrast have a beta of roughly 3 for a remarkably wide range of targets
% and conditions (Nachmias, 1981). However, you may want to estimate beta
% for the particular conditions of your experiment. QuestBetaAnalysis is
% provided for that purpose, but please think of it as a limited optional
% feature. It allows only two free parameters, threshold and beta. You may
% prefer to use a general-purpose maximum likelihood fitting program to
% allow more degrees of freedom in fitting a Weibull function to your
% psychometric data. However, once you've done that it's likely that
% you'll settle on fixed values for all but threshold and use Quest to
% estimate that.
% 
% Note that data collected to estimate threshold usually are not
% good for estimating beta. The psychometric function is sigmoidal, with a
% flat floor, a rise, and a flat ceiling. To estimate threshold you want
% all your trials near the steepest (roughly speaking) part of the rise.
% To estimate beta, the steepness of the rise, you want to have most of
% your trials at the corners, where the rise begins and where it ends. The
% usual way to achieve this is to first estimate threshold and then to
% collect a large number of trials (e.g. 100) at each of several
% intensities chosen to span the domain of the rise. These data can
% be plotted, making a nice graph of the psychometric function and
% they can be fed to QuestBetaAnalysis, to estimate threshold and beta.
% 
% References
% 
% Farell, B., & Pelli, D. G. (1999). Psychophysical methods, or how to
% measure threshold, and why. In J. G. Robson & R. H. S. Carpenter (Eds.),
% A Practical Guide to Vision Research (pp. 129-136). New York: Oxford
% University Press.
% 
% King-Smith, P. E., Grigsby, S. S., Vingrys, A. J., Benes, S. C., and
% Supowit, A. (1994) Efficient and unbiased modifications of the QUEST
% threshold method: theory, simulations, experimental evaluation and
% practical implementation. Vision Res, 34 (7), 885-912.
% 
% Nachmias, J. (1981). On the psychometric function for contrast detection. 
% Vision Res, 21(2), 215-223. 
% 
% Pelli, D. G. (1987) The ideal psychometric procedure. Investigative
% Ophthalmology & Visual Science, 28 (Suppl), 366.
% 
% Pelli, D. G., & Farell, B. (1994). Psychophysical methods. In M. Bass,
% E. W. Van Stryland, D. R. Williams & W. L. Wolfe (Eds.), Handbook of
% Optics, 2nd ed. (Vol. I, pp. 29.21-29.13). New York: McGraw-Hill.
% 
% Watson, A. B. and Pelli, D. G. (1983) QUEST: a Bayesian adaptive
% psychometric method. Percept Psychophys, 33 (2), 113-20.
% 
% All the papers of which I'm an author can be downloaded as PDF files
% from my web site:
% web http://psych.nyu.edu/pelli/
% 
% Try "help Quest".

% Copyright (c) 1996-2004 Denis G. Pelli
%
% 3/3/97  dhb  Cosmetic editing.
% 10/13/04 dgp Added paragraph noting that Quest's suggestion for next trial 
%              is merely a suggestion.
% 12/18/04 dgp Made it independent of the Psychtoolbox and greatly expanded the help text above.
% 12/20/04 dgp Added explanation to the print outs.
% 3/11/05  dgp Added one more decimal place to log C for each trial, as suggested by David Jones.
%              Loop INPUT until observer gives a value.

% GetSecs is part of the Psychophysics Toolbox.  If you are running 
% QuestDemo without the Psychtoolbox, we use CPUTIME instead of GetSecs.
if exist('GetSecs')
	getSecsFunction='GetSecs';
else
	getSecsFunction='cputime';
end

fprintf('Welcome to QuestDemo. Quest will now estimate an observer''s threshold.\n');
fprintf('The intensity scale is abstract, but usually we think of it as representing\n');
fprintf('log contrast. ');

% We'll need this for the simulation.
fprintf('Quest won''t know, but in this demo we''re testing a simulated observer. \n');
tActual=[];
while isempty(tActual)
	tActual=input('Please specify the true threshold of the simulated observer (e.g. -2): ');
end
fprintf('Thank you. We won''t tell Quest.\n');
fprintf('\nWhen you test a real human observer, instead of a simulated observer, \n');
fprintf('you won''t know the true threshold. However you can still guess. You \n');
fprintf('must provide Quest with an initial threshold estimate as a mean and \n');
fprintf('standard deviation, which we call your "guess" and "sd". Be generous \n');
fprintf('with the sd, as Quest will have trouble finding threshold if it''s more\n');
fprintf('than one sd from your guess.\n');

% Provide our prior knowledge to QuestCreate, and receive the data struct "q".
tGuess=[];
while isempty(tGuess)
	tGuess=input('Estimate threshold (e.g. -1): ');
end
tGuessSd=[];
while isempty(tGuessSd)
	tGuessSd=input('Estimate the standard deviation of your guess, above, (e.g. 2): ');
end
pThreshold=0.82;
beta=3.5;delta=0.01;gamma=0.5;
q=QuestCreate(tGuess,tGuessSd,pThreshold,beta,delta,gamma);
q.normalizePdf=1; % This adds a few ms per call to QuestUpdate, but otherwise the pdf will underflow after about 1000 trials.

% fprintf('Your initial guess was %g +- %g\n',tGuess,tGuessSd);
% fprintf('Quest''s initial threshold estimate is %g +- %g\n',QuestMean(q),QuestSd(q));

% Simulate a series of trials. 
% On each trial we ask Quest to recommend an intensity and we call QuestUpdate to save the result in q.
trialsDesired=40;
wrongRight={'wrong','right'};
timeZero=eval(getSecsFunction);
for k=1:trialsDesired
	% Get recommended level.  Choose your favorite algorithm.
	tTest=QuestQuantile(q);	% Recommended by Pelli (1987), and still our favorite.
	% 	tTest=QuestMean(q);		% Recommended by King-Smith et al. (1994)
	% 	tTest=QuestMode(q);		% Recommended by Watson & Pelli (1983)
	
	% We are free to test any intensity we like, not necessarily what Quest suggested.
	% 	tTest=min(-0.05,max(-3,tTest)); % Restrict to range of log contrasts that our equipment can produce.
	
	% Simulate a trial
	timeSplit=eval(getSecsFunction); % Omit simulation and printing from the timing measurements.
 	response=QuestSimulate(q,tTest,tActual);
 	fprintf('Trial %3d at %5.2f is %s\n',k,tTest,char(wrongRight(response+1)));
	timeZero=timeZero+eval(getSecsFunction)-timeSplit;
	
	% Update the pdf
	q=QuestUpdate(q,tTest,response); % Add the new datum (actual test intensity and observer response) to the database.
end

% Print results of timing.
fprintf('%.0f ms/trial\n',1000*(eval(getSecsFunction)-timeZero)/trialsDesired);

% Ask Quest for the final estimate of threshold.
t=QuestMean(q);		% Recommended by Pelli (1989) and King-Smith et al. (1994). Still our favorite.
sd=QuestSd(q);
fprintf('Final threshold estimate (mean+-sd) is %.2f +- %.2f\n',t,sd);
% t=QuestMode(q);	% Similar and preferable to the maximum likelihood recommended by Watson & Pelli (1983). 
% fprintf('Mode threshold estimate is %4.2f\n',t);
fprintf('\nYou set the true threshold to %.2f.\n',tActual);
fprintf('Quest knew only your guess: %.2f +- %.2f.\n',tGuess,tGuessSd);

% Optionally, reanalyze the data with beta as a free parameter.
fprintf('\nBETA. Many people ask, so here''s how to analyze the data with beta as a free\n');
fprintf('parameter. However, we don''t recommend it as a daily practice. The data\n');
fprintf('collected to estimate threshold are typically concentrated at one\n');
fprintf('contrast and don''t constrain beta. To estimate beta, it is better to use\n');
fprintf('100 trials per intensity (typically log contrast) at several uniformly\n');
fprintf('spaced intensities. We recommend using such data to estimate beta once,\n');
fprintf('and then using that beta in your daily threshold meausurements. With\n');
fprintf('that disclaimer, here''s the analysis with beta as a free parameter.\n');
QuestBetaAnalysis(q); % optional
fprintf('Actual parameters of simulated observer:\n');
fprintf('logC	beta	gamma\n');
fprintf('%5.2f	%4.1f	%5.2f\n',tActual,q.beta,q.gamma);
