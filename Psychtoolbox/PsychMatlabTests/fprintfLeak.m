function fprintfLeak
% Mathworks Case ID: 319124, reported 11/23/99.
% 
% FPRINTF leaks memory, as measured by Mac OS TempFreeMem.
%
% "Thank you for reporting this bug.  I will let our development team know,
% but at this time development has stopped for the OS9 platform, so no
% patches are expected." 
% 
% Megean McDuffy
% Technical Support Engineer
% mmcduffy@mathworks.com

fprintf('This program documents a memory leak in FPRINTF. The amount of \n');
fprintf('Temporary Free Memory available through the Mac OS is reduced \n');
fprintf('by each call to FPRINTF, by approximately the length of the formatted string plus 1.\n');
fprintf('The following example prints 9 bytes, 100 times, and loses a total of about 1000 bytes.\n\n');
free0=Bytes;
tempFree0=Bytes('TempFree');
for i=1:100
	fprintf('123456789');
end
free=Bytes;
tempFree=Bytes('TempFree');
fprintf('\n\n');
fprintf('Before: %8ld free, %8ld temp free.\n',free0,tempFree0);
fprintf('After:  %8ld free, %8ld temp free.\n',free,tempFree);
fprintf('Change: %8ld free, %8ld temp free.\n',free-free0,tempFree-tempFree0);
fprintf('\nThe most likely explanation is that FPRINTF is calling the Mac Toolbox function TempNewHandle()\n');
fprintf('to allocate space for the output string, and failing to call DisposeHandle().\n');
