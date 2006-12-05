/*
	PsychToolbox2/Source/Common/Screen/PsychGLGlue.c
	
	PLATFORMS:	Windows
				MacOS9
			
	
	AUTHORS:
	Allen Ingling		awi		Allen.Ingling@nyu.edu

	HISTORY:
	09/09/02			awi		wrote it.  
	
	DESCRIPTION:
	
	Functions to convert between Psych toolbox data types and GL data types.  	
        
        TO DO:
        
*/

#include "Screen.h"

/*
    PsychConvertColorAndDepthToDoubleVector()
    
    Accept a color structure and a screen depth and return either three or four double values in the range between
    0-1 which specify r, g, b and optinally alpha values.
    
    The value array argument should be be four elements long.
    
*/
int PsychConvertColorAndDepthToDoubleVector(PsychColorType *color, int depthValue, GLdouble *valueArray)
{
     GLdouble deno;
    
    deno= (1<<PsychGetColorSizeFromDepthValue(depthValue))-1;
        
    switch(color->mode){
        case kPsychIndexColor:
            valueArray[0]=color->value.index.i/deno;
            return(1);
        case kPsychRGBColor:
            valueArray[0]=color->value.rgb.r/deno;
            valueArray[1]=color->value.rgb.g/deno;
            valueArray[2]=color->value.rgb.b/deno;
            return(3); 
        case kPsychRGBAColor:
            valueArray[0]=color->value.rgba.r/deno;
            valueArray[1]=color->value.rgba.g/deno;
            valueArray[2]=color->value.rgba.b/deno;
            valueArray[3]=color->value.rgba.a/deno;
            return(4);
        case kPsychUnknownColor:
            PsychErrorExitMsg(PsychError_internal,"Unspecified display mode");
    }
    PsychErrorExitMsg(PsychError_internal,"Unknown display mode");
    return(0); //make the compiler happy.  
}



/*
    PsychConvertColorAndDepthToDoubleVector()
    
    Accept a color structure and a screen depth and return either three or four double values in the range between
    0-1 which specify r, g, b and optinally alpha values.
    
    The value array argument should be be four elements long.
    
*/
int PsychConvertColorAndColorSizeToDoubleVector(PsychColorType *color, int colorSize, GLdouble *valueArray)
{
     GLdouble deno;
    
    deno= (1<<colorSize)-1;
        
    switch(color->mode){
        case kPsychIndexColor:
            valueArray[0]=color->value.index.i/deno;
            return(1);
        case kPsychRGBColor:
            valueArray[0]=color->value.rgb.r/deno;
            valueArray[1]=color->value.rgb.g/deno;
            valueArray[2]=color->value.rgb.b/deno;
            return(3); 
        case kPsychRGBAColor:
            valueArray[0]=color->value.rgba.r/deno;
            valueArray[1]=color->value.rgba.g/deno;
            valueArray[2]=color->value.rgba.b/deno;
            valueArray[3]=color->value.rgba.a/deno;
            return(4);
        case kPsychUnknownColor:
            PsychErrorExitMsg(PsychError_internal,"Unspecified display mode");
    }
    PsychErrorExitMsg(PsychError_internal,"Unknown display mode");
    return(0); //make the compiler happy.  
}



/*
    PsychSetGLColor()
    
    Accept a Psych color structure and a depth value and call the appropriate variant of glColor.       
*/
void PsychSetGLColor(PsychColorType *color, int depthValue)
{
    GLdouble dVals[4]; 
    int numVals;
    
    numVals=PsychConvertColorAndDepthToDoubleVector(color, depthValue, dVals);
    if(numVals==1)
        PsychErrorExitMsg(PsychError_internal, "palette mode not yet implemented");
    else if(numVals==3)
        glColor3dv(dVals);
    else if(numVals==4)
        glColor4dv(dVals);
    else
        PsychErrorExitMsg(PsychError_internal, "Illegal color specifier"); 
}



/*
    PsychGLRect()
*/
void PsychGLRect(double *psychRect)
{
    glRectd((GLdouble)(psychRect[kPsychLeft]),
            (GLdouble)(psychRect[kPsychTop]),
            (GLdouble)(psychRect[kPsychRight]),
            (GLdouble)(psychRect[kPsychBottom]));
}


char *PsychGetGLErrorNameString(GLenum errorConstant)
{
    static char GL_NO_ERROR_str[] = "GL_NO_ERROR";
    static char GL_INVALID_ENUM_str[] = "GL_INVALID_ENUM";
    static char GL_INVALID_VALUE_str[] = "GL_INVALID_VALUE";
    static char GL_INVALID_OPERATION_str[] = "GL_INVALID_OPERATION";
    static char GL_STACK_OVERFLOW_str[] = "GL_STACK_OVERFLOW";
    static char GL_STACK_UNDERFLOW_str[] = "GL_STACK_UNDERFLOW";
    static char GL_OUT_OF_MEMORY_str[] = "GL_OUT_OF_MEMORY";
    static char GL_TABLE_TOO_LARGE_str[] = "GL_TABLE_TOO_LARGE";
    static char unrecognized_error_str[] = "unrecognized GL error constant";
    
    switch(errorConstant){
        case GL_NO_ERROR: return(GL_NO_ERROR_str);
        case GL_INVALID_ENUM: return(GL_INVALID_ENUM_str);
        case GL_INVALID_VALUE: return(GL_INVALID_VALUE_str);
        case GL_INVALID_OPERATION: return(GL_INVALID_OPERATION_str);
        case GL_STACK_OVERFLOW: return(GL_STACK_OVERFLOW_str);
        case GL_STACK_UNDERFLOW: return(GL_STACK_UNDERFLOW_str);
        case GL_OUT_OF_MEMORY: return(GL_OUT_OF_MEMORY_str);
        case GL_TABLE_TOO_LARGE: return(GL_TABLE_TOO_LARGE_str);
    }
    return(unrecognized_error_str);
           
}

/*
	PsychGetGLErrorListString()
	
*/ 
Boolean PsychGetGLErrorListString(const char **errorListStr)
{
	#define MAX_GL_ERROR_LIST_LEN			2048
	#define MAX_GL_ERROR_LIST_DELTA_LEN		256
	static char	errorListString[MAX_GL_ERROR_LIST_LEN];
	char	*errorNameStr;
	int		currentIndex, deltaStrLen, nextCurrentIndex;
    GLenum	glError;
    boolean	isError=FALSE;
	
	currentIndex=0;
    for(glError=glGetError(); glError!=GL_NO_ERROR; glError=glGetError()){
		errorNameStr=PsychGetGLErrorNameString(glError);
		deltaStrLen=strlen(errorNameStr)+2;  //2 chars: comma and space
		nextCurrentIndex=currentIndex+deltaStrLen;
		if(nextCurrentIndex >= MAX_GL_ERROR_LIST_LEN)
			PsychErrorExitMsg(PsychError_internal,"string memory overflow");
		if(isError)
			sprintf(&(errorListString[currentIndex]), " ,%s", errorNameStr);
		else
			sprintf(&(errorListString[currentIndex]), "%s", errorNameStr);
		currentIndex=nextCurrentIndex;
		isError=TRUE;		
	}
	if(isError)
		*errorListStr=errorListString;
	else
		*errorListStr=NULL;
	return(isError);
}

 


 void PsychTestForGLErrorsC(int lineNum, const char *funcName, const char *fileName)
{
    boolean			isError;
	const char		*glErrorListString;
    
	isError=PsychGetGLErrorListString(&glErrorListString);
	if(isError)
		PsychErrorExitC(PsychError_OpenGL, 
						glErrorListString, 
						lineNum, 
						funcName, 
						fileName);
}
						
						 
						


/*
	PsychExtractQuadVertexFromRect()
	
	Return one of the four vertices define by a Psych rect in a 2-element array of GLdoubles.
	Vertices are numbered from the top left corner (0) clockwise to the bottom left corner (3).
*/
GLdouble *PsychExtractQuadVertexFromRect(double *rect, int vertexNumber, GLdouble *vertex)
{
	switch(vertexNumber){
		case 0:
			vertex[0]=(GLdouble)rect[0];
			vertex[1]=(GLdouble)rect[1];
			break;
		case 1:
			vertex[0]=(GLdouble)rect[2];
			vertex[1]=(GLdouble)rect[1];
			break;
		case 2:
			vertex[0]=(GLdouble)rect[2];
			vertex[1]=(GLdouble)rect[3];
			break;
		case 3:
			vertex[0]=(GLdouble)rect[0];
			vertex[1]=(GLdouble)rect[3];
			break;
		default:
			PsychErrorExitMsg(PsychError_internal, "Illegal vertex value");
	}
	return(vertex);
}

    



