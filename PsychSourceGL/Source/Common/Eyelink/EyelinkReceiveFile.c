/* 
    /osxptb/trunk/PsychSourceGL/Source/OSX/Eyelink/EyelinkReceiveFile.c
 
	PROJECTS: Eyelink 
 
	AUTHORS:
    E.Peters@ai.rug.nl				emp
    f.w.cornelissen@med.rug.nl		fwc
 
	PLATFORMS:	Currently only OS X  
 
	HISTORY:
 
	29-05-2001	emp 		created it
	28/06/06	fwc		adapted from alpha version
	30/10/24	br		adapted <options> in place of <dist_is_path> for EL3 support
 
	TARGET LOCATION:
 
 Eyelink.mexmac resides in:
 PsychHardware/EyelinkToolbox/EyelinkBasic
 
 WARNING, Maybe the implementation of this routine works only on the Macintosh
 
 */
 
#include "PsychEyelink.h"
 
 static char useString[] = "[status =] Eyelink('ReceiveFile', ['src'], ['dest'], ['options'])";
 
 static char synopsisString[] = 
 " If <src> filename is omitted or empty, tracker will send last opened data file.\n"
 " If <dest> is omitted, creates local file with source file name.\n"
 " Else, creates file using <dest> as name.\n"
 " <options>\n"
 "  0 files placed at the same folder where it executes\n"
 "  1 appends file name to <dest> as a directory path.\n"
 "  2 downlod vFile in addition to edf\n"
 "  3 download vFile & edf and append file name to <dest> as a directory path.\n"
 "  4 download only vFile and place it in the same folder\n"
 "  5 download only vFile and append file name to <dest> as a directory path\n"
 " Returns:\n"
 "  0 if file transfer was cancelled.\n"
 "  Size of file in bytes, if successful.\n"
 "  -1 if file size is negative.\n"
 "  -2 if cannot create local file.\n"
 "  -3 if file transfer was cancelled.\n"
 "  -4 if file transfer was aborted.\n"
 "  -5 if error occurred while writing file.\n"
 "  -6 if link was terminated.";

 
 
 static char seeAlsoString[] = "";
 
 /*
  ROUTINE: EyelinkReceiveFile
  PURPOSE:
 " If <src> is omitted, tracker will send last opened data file.\n"
 " If <dest> is omitted, creates local file with source file name.\n"
 " Else, creates file using <dest> as name.\n"
 " <options>"
 "  0 files placed at the same folder where it executes\n"
 "  1 appends file name to <dest> as a directory path.\n"
 "  2 downlod vFile in addition to edf\n"
 "  3 download vFile & edf and append file name to <dest> as a directory path.\n"
 "  4 download only vFile and place it in the same folder\n"
 "  5 download only vFile and append file name to <dest> as a directory path\n"
 " Returns:\n"
 "  0 if file transfer was cancelled due to edf file missing at tracker.\n"
 "  Size of file in bytes, if successful.\n"
 "  -1 if file size is negative.\n"
 "  -2 if cannot create local file.\n"
 "  -3 if file transfer was cancelled.\n"
 "  -4 if file transfer was aborted.\n"
 "  -5 if error occurred while writing file.\n"
 "  -6 if link was terminated."*/

 static short ELCALLTYPE mex_printf_progress(char * fileName, UINT64 size, UINT64 recv)
 {
	mexPrintf("Transferring: %s -- %zu/%zu\n", fileName, recv, size);
 }
 
 PsychError EyelinkReceiveFile(void)
 {
	 int iStatus = -1;
	 char *src = "";
	 char *dest = "";
	 int options = 0;
	 
	 //all sub functions should have these two lines
	 PsychPushHelp(useString, synopsisString, seeAlsoString);
	 if(PsychIsGiveHelp()){
		 PsychGiveHelp();
		 return(PsychError_none);
	 };
	 
	 //check to see if the user supplied superfluous arguments
	 PsychErrorExit(PsychCapNumInputArgs(3));
	 PsychErrorExit(PsychRequireNumInputArgs(0));
	 PsychErrorExit(PsychCapNumOutputArgs(1));
	 
	 // Verify eyelink is up and running
	 EyelinkSystemIsConnected();
	 EyelinkSystemIsInitialized();
	 
	 PsychAllocInCharArg(1, FALSE, &src);
	 PsychAllocInCharArg(2, FALSE, &dest);
	 PsychCopyInIntegerArg(3, FALSE, &options);

	 iStatus = (int) receive_data_file_feedback64(src, dest, (INT16) options, mex_printf_progress);

	 /* if there is an output variable available, assign result to it.   */			
	 PsychCopyOutDoubleArg(1, FALSE, iStatus);
	 
	 return(PsychError_none);
 }
