function betaEstimate=QuestBetaAnalysis(q,fid)
% betaEstimate=QuestBetaAnalysis(q,[fid]);
% 
% Analyzes the quest function with beta as a free parameter. It prints (in
% the file or files pointed to by fid) the mean estimates of alpha (as
% logC) and beta. Gamma is left at whatever value the user fixed it at.
% 
% Note that normalization of the pdf, by QuestRecompute, is disabled because it
% would need to be done across the whole q vector. Without normalization,
% the pdf tends to underflow at around 1000 trials. You will have some warning
% of this because the printout mentions any values of beta that were dropped 
% because they had zero probability. Thus you should keep the number of trials
% under around 1000, to avoid the zero-probability warnings.
% 
% See Quest.

% Denis Pelli 5/6/99
% 8/23/99 dgp streamlined the printout
% 8/24/99 dgp add sd's to printout
% 10/13/04 dgp added comment explaining 1/beta
if nargin<1 || nargin>2
	error('Usage: QuestBetaAnalysis(q,[fid])')
end
if nargin<2
	fid=1;
end
fprintf('Now re-analyzing with both threshold and beta as free parameters. ...\n');
for f=fid
	fprintf(f,'logC 	 +-sd 	 beta	 +-sd	 gamma\n');
end
for i=1:length(q(:))
	betaEstimate(i)=QuestBetaAnalysis1(q(i),fid);
end
return

function betaEstimate=QuestBetaAnalysis1(q,fid)
for i=1:16
	q2(i)=q;
	q2(i).beta=2^(i/4);
	q2(i).dim=250;
	q2(i).grain=0.02;
end
qq=QuestRecompute(q2);

% omit betas that have zero probability
for i=1:length(qq)
	p(i)=sum(qq(i).pdf);
end
if any(p==0)
	fprintf('Omitting beta values ');
	fprintf('%.1f ',qq(find(p==0)).beta);
	fprintf('because they have zero probability.\n');
end
clear q2
q2=qq(find(p));

t2=QuestMean(q2); % estimate threshold for each possible beta
p2=QuestPdf(q2,t2); % get probability of each of these (threshold,beta) combinations
sd2=QuestSd(q2); % get sd of threshold for each possible beta
beta2=[q2.beta];
% for f=fid
% 	fprintf(f,'beta ');fprintf(f,'	%7.1f',q2(:).beta);fprintf(f,'\n');
% 	fprintf(f,'t    ');fprintf(f,'	%7.2f',t2);fprintf(f,'\n');
% 	fprintf(f,'sd   ');fprintf(f,'	%7.2f',sd2);fprintf(f,'\n');
% 	fprintf(f,'log p');fprintf(f,'	%7.2f',log10(p2));fprintf(f,'\n');
% end
[p,i]=max(p2); % take mode, i.e. the most probable (threshold,beta) combination
t=t2(i); % threshold at that mode
sd=QuestSd(q2(i)); % sd of threshold estimate at the beta of that mode
p=sum(p2);
betaMean=sum(p2.*beta2)/p;
betaSd=sqrt(sum(p2.*beta2.^2)/p-(sum(p2.*beta2)/p).^2);
% beta has a very skewed distribution, with a long tail out to very large value of beta, whereas 1/beta is 
% more symmetric, with a roughly normal distribution. Thus it is statistically more efficient to estimate the
% parameter as 1/average(1/beta) than as average(beta). "iBeta" stands for inverse beta, 1/beta.
% The printout takes the conservative approach of basing the mean on 1/beta, but reporting the sd of beta.
iBetaMean=sum(p2./beta2)/p;
iBetaSd=sqrt(sum(p2./beta2.^2)/p-(sum(p2./beta2)/p).^2);
for f=fid
	%	fprintf(f,'Threshold %4.2f +- %.2f; Beta mode %.1f mean %.1f +- %.1f imean 1/%.1f +- %.1f; Gamma %.2f\n',t,sd,q2(i).beta,betaMean,betaSd,1/iBetaMean,iBetaSd,q.gamma);
	%	fprintf(f,'%5.2f	%4.1f	%5.2f\n',t,1/iBetaMean,q.gamma);
	fprintf(f,'%5.2f	%5.2f	%4.1f	%4.1f	%6.3f\n',t,sd,1/iBetaMean,betaSd,q.gamma);
end
betaEstimate=1/iBetaMean;
