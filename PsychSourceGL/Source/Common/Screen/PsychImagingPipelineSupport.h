/*
	PsychToolbox3/Source/Common/Screen/PsychImagingPipelineSupport.h
	
	PLATFORMS:	
	
		All.  
				
	AUTHORS:
	
		Mario Kleiner           mk              mario.kleiner at tuebingen.mpg.de

	HISTORY:
	
		12/05/06	mk	Wrote it.
		
	DESCRIPTION:
	
		Infrastructure for all Screen imaging pipeline functions, i.e., hook callback functions and chains
		and GLSL based internal image processing pipeline.
		
	NOTES:
	
	TO DO: 
	
*/

//include once
#ifndef PSYCH_IS_INCLUDED_PsychImagingPipelineSupport
#define PSYCH_IS_INCLUDED_PsychImagingPipelineSupport

#include "Screen.h"

// Initialize imaging pipeline for windowRecord, applying the imagingmode flags. Called by Screen('OpenWindow')
void	PsychInitializeImagingPipeline(PsychWindowRecordType *windowRecord, int imagingmode);

//end include once
#endif

