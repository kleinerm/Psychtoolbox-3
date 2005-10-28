function ylabelBug
% Mathworks Case ID: 297565, submitted 10/4/99.
% 
% YLABEL command (in Matlab plots) bug and work around.
% 
% dear mathworks
% 
% there's a bug in the YLABEL command. Here's a tiny program that 
% demonstrates the bug.
% 
% % c vs k
% k=1:100;
% c=1./k;
% loglog(k,c)
% %xlabel('Extent (i.e. number of 
% features)','FontSize',14,'FontWeight','bold');
% ylabel('Threshold contrast','FontSize',14,'FontWeight','bold');
% 
% The plot looks ok in Matlab, but if you copy and paste it into any other 
% application, eg Microsoft PowerPoint, the YLABEL command results in a 
% spurious "TextEnd" error message in the middle of the graph. The TITLE 
% and 
% XLABEL commands don't have this problem.
% 
% I'm using Matlab 5.2.1 on a PowerMac, Mac OS 8.6. Is there any 
% workaround? I need y labels.
% 
% best
% 
% denis pelli
% professor of psychology and neural science at nyu
% apple developer
% 
% Reported as a bug to Mathworks on 10/4/99, Case ID: 297565. Their reply (10/4) follows:
% 
% Dear Denis,
% 
% I am writing in response to your e-mail regarding YLABEL. I believe the
% following work around will help you.
% 
% This is a bug and it has been reported to our development staff. 
% 
% If you are printing to a Postscript printer, then the work around is to
% attach Postscript to the PICT file.  
% 
% To do this:
% 
% 1)  Choose the Output Preferences item from the Options menu in Matlab.  
% 
% 2)  In this dialog box, check the  Honor Figure Position checkbox.
% 
% 3)  Select Copy PICT with EPS as the default format.  
% 
% Then you can copy and paste a graph into another application which will
% result in proper output.
% 
% If this does not work for you, here is another technical note which may
% pertain to your problem:
% 
% web http://www.mathworks.com/support/solutions/v5/22133.shtml
% 
% I hope that this helps you!
% 
% Sincerely,
% 
% Sean Cote
% Technical Support Engineer
% scote@mathworks.com

% c vs k, summation
k=1:100;
c=1./k;
loglog(k,c)
%xlabel('Extent (i.e. number of features)','FontSize',14,'FontWeight','bold');
ylabel('Threshold contrast','FontSize',14,'FontWeight','bold');

fprintf('The plot looks ok in Matlab, but if you copy and paste it into any other\n');
fprintf('application, eg Microsoft PowerPoint, the YLABEL command results in a \n');
fprintf('spurious "TextEnd" error message in the middle of the graph. The TITLE and \n');
fprintf('XLABEL commands don''t have this problem.\n');
