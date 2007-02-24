/*
	PsychToolbox2/Source/Common/WindowBank.cpp		

	AUTHORS:
	
		Allen.Ingling@nyu.edu		awi 

	PLATFORMS: 
	
		Only OS X for now.


	HISTORY:
	
		07/18/02  awi		Wrote it. This is a second attempt.  The first
							attempted to maintain state with static
							variables internal to functions. Much too 
							awkward for prototyping purposes.  
                07/22/05  mk            Windowbank array is allocated and resized dynamically now, so no limit to maximum
                                        number of windows anymore.

	DESCRIPTION:

		WindowBank contains functions for storing records of open 
		onscreen and offscreen windows.  

		See WindowHelpers for convenience functions which access the fields 
		of records defined in WindowBank.h

		TO DO:

		Change the window index returned to the scripting environment to be the address of the window record.     

*/

#include "Screen.h"


/* ------------------------------------------------------------------ 
	Declare static persistant variables local to WindwBank.cpp variables. 
*/

// MK: windowRecordArrayWINBANK is allocated in InitWindowBank(), released in CloseWindowBank(),
// and dynamically resized in PsychCreateWindowArrayRecord(), if necessary to accomodate more windows.
// We resize in chunks of PSYCH_ALLOC_WINDOW_RECORDS_INC window slots to avoid/reduce possible memory-
// fragmentation and make allocation efficient. This approach is better than using linked-lists, because
// it is easier to implement and still allows access to each windowRecord in constant time via array-lookup,
// avoiding increasing access-delays that would happen with long linked lists.
static PsychWindowRecordType **windowRecordArrayWINBANK=NULL;
static PsychScreenRecordType *screenRecordArrayWINBANK[PSYCH_ALLOC_SCREEN_RECORDS];
static int numWindowRecordsWINBANK=0;

// MK: See InitWindowBank() for initial setup of variables below...
static int PSYCH_MAX_WINDOWS=0; // Maximum number of onscreen-/offscreen-windows/textures...	
static int PSYCH_LAST_WINDOW=0; // the highest possible window index			
static int PSYCH_ALLOC_WINDOW_RECORDS=0; //current length of dynamic array allocated to hold window pointers.	

#define PSYCH_ALLOC_WINDOW_RECORDS_INC 4096  // Increment when extending the window bank...

//Local function prototypes
PsychWindowIndexType FindEmptyWindowIndex(void);


//	Window accessor functions for the outside world. 


/*
	PsychFindScreenWindowFromScreenNumber()
	
	Walk down the list of onscreen windows looking for the window open on the specified screen and and set *winRec to point to its window recored if we find one.
	Oherwise set *winRec to NULL.  
	
	This could be improved by storing the pointer to the onscreen window in the screen record.  
*/

void PsychFindScreenWindowFromScreenNumber(int screenNumber, PsychWindowRecordType **winRec)
{
	int							i, numWindows;
	PsychWindowRecordType		**windowArray;
	
	*winRec=NULL;
	if(screenNumber==kPsychUnaffiliatedWindow)
		return;
	PsychCreateVolatileWindowRecordPointerList(&numWindows, &windowArray);
	for(i=0;i<numWindows;i++){
		if(PsychIsOnscreenWindow(windowArray[i])){
			if(windowArray[i]->screenNumber==screenNumber){
				*winRec=windowArray[i];
				break;
			}
		}
	}
	PsychDestroyVolatileWindowRecordPointerList(windowArray);
	
} 



/* 
	Initialize list of window record pointers.
*/
PsychError InitWindowBank(void)
{
	PsychWindowIndexType i;
        
        PSYCH_ALLOC_WINDOW_RECORDS=PSYCH_ALLOC_WINDOW_RECORDS_INC; // Initial length of array allocated to hold window pointers.	
    
        // MK: Allocate an initial windowbank of default size PSYCH_ALLOC_WINDOW_RECORDS:
        windowRecordArrayWINBANK=malloc(PSYCH_ALLOC_WINDOW_RECORDS * sizeof(PsychWindowRecordType*));
	if (windowRecordArrayWINBANK==NULL) {
            // Out of memory!
            return(PsychError_outofMemory);
        }
        
        PSYCH_MAX_WINDOWS=PSYCH_ALLOC_WINDOW_RECORDS-10;
        PSYCH_LAST_WINDOW=PSYCH_ALLOC_WINDOW_RECORDS-1; //the highest possible window index			
        
        // Initialize with NULL-Ptrs:
        for(i=PSYCH_FIRST_WINDOW;i<=PSYCH_LAST_WINDOW;i++)
		windowRecordArrayWINBANK[i] = NULL;
        
        return(PsychError_none); //no error
}

 
/*
	Free storage for window records.  All windows should be
	close first. 
*/
PsychError CloseWindowBank(void)
{
	PsychWindowIndexType i;
	
	for(i=PSYCH_FIRST_WINDOW;i<=PSYCH_LAST_WINDOW;i++){
            if(windowRecordArrayWINBANK[i] != NULL)
                    free(windowRecordArrayWINBANK[i]);
        }
	
        // MK: Release the array of pointers itself:
        free(windowRecordArrayWINBANK);

        windowRecordArrayWINBANK=NULL;
        PSYCH_MAX_WINDOWS=0;
        PSYCH_LAST_WINDOW=0;			
        PSYCH_ALLOC_WINDOW_RECORDS=0;	
        
        return(PsychError_none);
}

/* 
	Count the number of windows of the specified type.
*/
int PsychCountOpenWindows(PsychWindowType winType)
{
	int i, total=0;
	
	for(i=PSYCH_FIRST_WINDOW;i<=PSYCH_LAST_WINDOW;i++){
            if(windowRecordArrayWINBANK[i]){
                if(winType==kPsychAnyWindow)
                    ++total;
                else if(windowRecordArrayWINBANK[i]->windowType==winType)
                    ++total;
		}
	}
	return(total);
}


/*
        PsychIsLastWindow()
        
        Is this the last open onscreen window for this screen ?
        SCREENClose needs to know this so that it can release the screen 
*/
boolean PsychIsLastOnscreenWindow(PsychWindowRecordType *windowRecord)
{
    int i;

    if(!PsychIsOnscreenWindow(windowRecord))
        return(FALSE);
    for(i=PSYCH_FIRST_WINDOW;i<=PSYCH_LAST_WINDOW;i++){
        if(windowRecordArrayWINBANK[i]){
            if(windowRecordArrayWINBANK[i]->screenNumber == windowRecord->screenNumber  &&  
                windowRecordArrayWINBANK[i]->windowIndex != windowRecord->windowIndex	&&
                (windowRecordArrayWINBANK[i]->windowType == kPsychSingleBufferOnscreen || 
                windowRecordArrayWINBANK[i]->windowType == kPsychDoubleBufferOnscreen))
                return(FALSE);
        }
    }
    return(TRUE);
}


/* 
	Allocate and return an empty window record 
*/
void PsychCreateWindowRecord(PsychWindowRecordType **winRec)
{
        PsychWindowRecordType **tmpwindowRecordArrayWINBANK=NULL;
        PsychWindowIndexType i;
    
        //check for space
        if(numWindowRecordsWINBANK==PSYCH_MAX_WINDOWS) {
            // Windowbank - array is full! We reallocate it, extending it
            // by PSYCH_ALLOC_WINDOW_RECORDS_INC additional slots for additional windows.
            tmpwindowRecordArrayWINBANK=realloc(windowRecordArrayWINBANK, (PSYCH_ALLOC_WINDOW_RECORDS + PSYCH_ALLOC_WINDOW_RECORDS_INC) * sizeof(PsychWindowRecordType*));
            if (tmpwindowRecordArrayWINBANK==NULL) {
                // realloc() failed due to out-of-memory!
		PsychErrorExit(PsychError_outofMemory);   //out of memory
            }

            // Success! Update limits and initialize new slots:
            windowRecordArrayWINBANK = tmpwindowRecordArrayWINBANK;
            PSYCH_ALLOC_WINDOW_RECORDS+=PSYCH_ALLOC_WINDOW_RECORDS_INC;
            PSYCH_MAX_WINDOWS+=PSYCH_ALLOC_WINDOW_RECORDS_INC;
            // Initialize new slots with NULL-Ptrs:
            i=PSYCH_LAST_WINDOW + 1;
            PSYCH_LAST_WINDOW+=PSYCH_ALLOC_WINDOW_RECORDS_INC;
            for(;i<=PSYCH_LAST_WINDOW;i++) windowRecordArrayWINBANK[i] = NULL;
            // Ready for addition of new windows.
        }
    	
	//allocate storage
	if( (*winRec=(PsychWindowRecordType *)malloc(sizeof(PsychWindowRecordType))) == NULL )
		PsychErrorExit(PsychError_outofMemory);   //out of memory
	
	//increment counts	
	++numWindowRecordsWINBANK; 
		
	//store the record at a free pointer index and set the records field to the index.  
	(*winRec)->windowIndex = FindEmptyWindowIndex();
	windowRecordArrayWINBANK[(*winRec)->windowIndex] = *winRec;
        
	//set a flag to indicate that the contents of the window record are not completely valid.
	(*winRec)->isValid=FALSE;
        
	// Set window type to kPsychNoWindow to signal that the window doesn't yet exist.
	// Although the windowRecord is partially initialized, there's no Windowsystem onscreen
	// window associated with this and the OpenGL context isn't there either. This is important
	// for error-handling. Windows of type kPsychNoWindow are ignored by the OpenGL and Window system
	// cleanup routine PsychCloseWindow()...
	(*winRec)->windowType = kPsychNoWindow;

	// Assign default number of color channels: 4 is a good number (RGBA), but this
	// gets overwritten in appropriate places...
	(*winRec)->nrchannels=4;
	
	// Initialize all imaging pipeline related fields to safe defaults:
	PsychInitImagingPipelineDefaultsForWindowRecord(*winRec);
	
	//Intialize the text settings field within the window record to default values which should be (but are not yet) specified in Psychtoolbox preferences.
	PsychInitTextRecordSettings(&((*winRec)->textAttributes));
	
	//Initialize the fields about textures.  
	PsychInitWindowRecordTextureFields(*winRec);
	
	//Initialize the fields used to store alpha blending factors as set by glBlendFunc()
	PsychInitWindowRecordAlphaBlendingFactors(*winRec);
        
	//Initialize line stipple values
	(*winRec)->stipplePattern=0xAAAA;		//alternating pixels stipple pattern
	(*winRec)->stippleFactor=1;
	(*winRec)->stippleEnabled=FALSE;

        // Initialize stereo settings:
		  (*winRec)->stereomode=0;
        (*winRec)->stereodrawbuffer=2;                  // No stero drawbuffer selected at window open time.
        (*winRec)->auxbuffer_dirty[0]=FALSE;            // AUX-Buffers clean on startup.
        (*winRec)->auxbuffer_dirty[1]=FALSE;
	// Initialize gamma table pointers for deferred loading:
	(*winRec)->inRedTable = NULL;
	(*winRec)->inGreenTable = NULL;
	(*winRec)->inBlueTable = NULL;
	(*winRec)->loadGammaTableOnNextFlip = 0;
}


/*
	Free the window record memory and set the corresponding window array entry to null.
	Its up to the calling function to close the window FIRST.
*/
PsychError FreeWindowRecordFromIndex(PsychWindowIndexType windex)
{	
	
	
	if(windex < PSYCH_FIRST_SCREEN)
		return(PsychError_scumberNotWindex); //I was passed a screen number, not a window index
	if(windex <= PSYCH_LAST_SCREEN)
		return(PsychError_scumberNotWindex); //I was passed a screen number, not a window pointer
	if(windowRecordArrayWINBANK[windex] ==NULL)
		return(PsychError_invalidWindex);    //window does not exist
		
	// Release temporary gamma tables, if any:
	free(windowRecordArrayWINBANK[windex]->inRedTable);
	free(windowRecordArrayWINBANK[windex]->inGreenTable);
	free(windowRecordArrayWINBANK[windex]->inBlueTable);

	free(windowRecordArrayWINBANK[windex]);
	windowRecordArrayWINBANK[windex] = NULL;
	--numWindowRecordsWINBANK;
	return(PsychError_none);
}

/*
	Free the window record memory and set the corresponding window array entry to null.
	Its up to the calling function to close the window FIRST.
*/
PsychError FreeWindowRecordFromPntr(PsychWindowRecordType *winRec)
{
	PsychWindowIndexType windowIndex;
	
	if(winRec == NULL)
		return(PsychError_nullWinRecPntr); //passed a null window pointer
	
	windowIndex = winRec->windowIndex;
	return(FreeWindowRecordFromIndex(windowIndex));
}



/*
	Accept a window pointer or a screen number (psychIndex) and return true if its a valid screen
	number and false otherwise
*/
boolean IsValidScreenNumber(PsychNumdexType numdex)
{
	return((int)((numdex >=PSYCH_FIRST_SCREEN) && (numdex < PsychGetNumDisplays())));

}


/*
	IsValidScreenNumberOrUnaffiliated()
	
	Accept a window pointer or a screen number (psychIndex) and return true iff its a valid screen
	number or is the token indicating an unaffiliated window. 
*/
boolean IsValidScreenNumberOrUnaffiliated(PsychNumdexType numdex)
{
	return(((int)((numdex >=PSYCH_FIRST_SCREEN) && (numdex < PsychGetNumDisplays()))) || numdex == kPsychUnaffiliatedWindow);

}


/*
	IsScreenNumberUnaffiliated()

*/
boolean IsScreenNumberUnaffiliated(PsychNumdexType numdex)
{
	return(numdex == kPsychUnaffiliatedWindow);

}



/*
	Accept a window pointer or a screen number and return true if its a valid window pointer
	number and false otherwise
*/
boolean IsWindowIndex(PsychNumdexType numdex)
{


	if(numdex<PSYCH_FIRST_WINDOW || numdex>PSYCH_LAST_WINDOW)
		return(FALSE);  //outside of index range
		
	//check to see if their is window record there.
	if(windowRecordArrayWINBANK[numdex] != NULL)
		return(1);
	
	return(0);		
}


/*
	Accept a window index and return a pointer to the window record, which will be NULL if no
	window exists.  We return an error if the index is invalid.
        
        This function also checks the valid flag in the window to make sure that that fields were
        successfully filled when the window was created. 
        
        All retrieval of window records should go through this function because it checks for valid
        windows.
        
*/
PsychError FindWindowRecord(PsychWindowIndexType windowIndex, PsychWindowRecordType **windowRecord)
{
	//check for valid index
	if(windowIndex<PSYCH_FIRST_WINDOW || windowIndex>PSYCH_LAST_WINDOW)
		return(PsychError_invalidWindex); //invalid index
		
	*windowRecord = windowRecordArrayWINBANK[windowIndex];
        PsychCheckIfWindowRecordIsValid(*windowRecord);  //this would fail because of an early exit when the window was created.

	return(PsychError_none);
}


/*
	Accept a screen number and return the corresponding screen record
*/
PsychError FindScreenRecord(int screenNumber, PsychScreenRecordType **screenRecord)
{
    if(IsValidScreenNumber(screenNumber)){
            *screenRecord = screenRecordArrayWINBANK[screenNumber];
            return(PsychError_none);
    }else
            return(PsychError_invalidScumber); 	
    
}

/*
    PsychCreateVolatileWindowRecordPointerList()
    
    Allocates memory for and returns an array holding indices of all open windows.  
    
    This is kind of lame.  It iterates over the window list twice.  The window bank stuff needs to be redone anyway.
	
	We don't really have to worry about deallocating this memory because MATLAB will garbage collect it  when 
	the Psychtoolbox call returns.  
    
*/
void PsychCreateVolatileWindowRecordPointerList(int *numWindows, PsychWindowRecordType ***pointerList)
{
    int 			i,j=0;
    PsychWindowRecordType	**tempList; 
    
    *numWindows=0;
    for(i=PSYCH_FIRST_WINDOW;i<=PSYCH_LAST_WINDOW;i++){
        if(windowRecordArrayWINBANK[i])
            ++(*numWindows);
    }
    tempList=(PsychWindowRecordType **)mxMalloc(sizeof(PsychWindowRecordType *) * *numWindows);
    for(i=PSYCH_FIRST_WINDOW;i<=PSYCH_LAST_WINDOW;i++){
        if(windowRecordArrayWINBANK[i])
            tempList[j++]=windowRecordArrayWINBANK[i];
    }
    *pointerList=tempList;     
}


/*
    PsychDestroyVolatileWindowIndexList()
*/
void PsychDestroyVolatileWindowRecordPointerList(PsychWindowRecordType **pointerList)
{
    mxFree((void *)pointerList);
}



//  ------------------------------------------------------------------
//	Accessor functions for stuff internal to WindowBank.cpp.   
//

PsychWindowIndexType FindEmptyWindowIndex(void)
{
	int i;
	for(i=PSYCH_FIRST_WINDOW;i<=PSYCH_LAST_WINDOW;i++){
		if(windowRecordArrayWINBANK[i]==NULL)
			return(i);
	}
	PsychErrorExitMsg(PsychError_toomanyWin,NULL);
	return(PSYCH_INVALID_WINDEX);
}


