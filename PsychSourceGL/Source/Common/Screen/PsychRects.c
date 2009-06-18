/*
	PsychRects.c
	
	PLATFORMS:	
	
		Only OS X for now. 
	
	AUTHORS:
	
		Allen Ingling		awi		Allen.Ingling@nyu.edu

	HISTORY:
	
		02/11/03			awi		wrote it.
		01/20/09			mk		Modify behaviour of PsychGetCenterFromRectAbs(): No longer floor() the size of the rectangle
									before calculation of center: This removes information about rects with fractional locations
									for no reason. Function is currently used by FillOval, FrameOval, Frame/Fill/xxxArc(), where
									it has harmful effect on accuracy and consistency.
        
	DESCRIPTION:	

*/

#include "Screen.h"


/*
	PsychMakeRect() 
	
	Convenience function for assembling PsychRects.  The caller alloctes the rect.  
*/
void PsychMakeRect(double *psychRect, double left, double top, double right, double bottom)
{
	psychRect[kPsychLeft]=left;			
	psychRect[kPsychTop]=top;			
	psychRect[kPsychRight]=right;			
	psychRect[kPsychBottom]=bottom;			
}

/*
	Check that the left side is on the left and the top is on the top and 
	the volume is >= 1 pixel.
*/

psych_bool IsPsychRectEmpty(PsychRectType rect)
{
        return((rect[kPsychLeft] == rect[kPsychRight] || rect[kPsychTop]==rect[kPsychBottom]));
}

psych_bool ValidatePsychRect(double *rect)
{
	if(rect[kPsychLeft] > rect[kPsychRight] || rect[kPsychTop]>rect[kPsychBottom])
		return(FALSE);
	return(TRUE);
}


/*
    PsychGetWidthFromRect()
*/
double PsychGetWidthFromRect(const double *rect)
{
    return(rect[kPsychRight]-rect[kPsychLeft]);
}

/*
    PsychGetHeightFromRect()
*/
double PsychGetHeightFromRect(const double *rect)
{
    return(rect[kPsychBottom]-rect[kPsychTop]);

}

/*
    PsychCenterRect()
    
    Centers one rect within the other.  The caller should allocate memory for the return value. 
*/

void PsychCenterRect(const double *innerRect, const double *outerRect, double *newRect)
{
    double leftOffset, topOffset, rightOffset, bottomOffset;
    
    rightOffset = innerRect[kPsychRight] - innerRect[kPsychLeft];
    bottomOffset =  innerRect[kPsychBottom] - innerRect[kPsychTop]; 
    leftOffset = floor(((outerRect[kPsychRight] - outerRect[kPsychLeft] ) - rightOffset)/2);
    topOffset = floor(((outerRect[kPsychBottom] - outerRect[kPsychTop] ) - bottomOffset)/2);
    newRect[kPsychRight] = newRect[kPsychLeft] + rightOffset; 
    newRect[kPsychBottom] = newRect[kPsychTop] + bottomOffset; 
    newRect[kPsychLeft] = outerRect[kPsychLeft] + leftOffset; 
    newRect[kPsychTop] = outerRect[kPsychTop] + topOffset; 
}

void PsychCenterRectInRect(const double *innerRect, const double *outerRect, double *newRect)
{
	double innerWidth, innerHeight, outerWidth, outerHeight;
	
	innerWidth=PsychGetWidthFromRect(innerRect);
	innerHeight=PsychGetHeightFromRect(innerRect);
	outerWidth=PsychGetWidthFromRect(outerRect);
	outerHeight=PsychGetHeightFromRect(outerRect);
	newRect[kPsychLeft]=floor((outerWidth - innerWidth) / 2.0);
	newRect[kPsychTop]=floor((outerHeight - innerHeight) / 2.0);
	newRect[kPsychRight]=newRect[kPsychLeft]+innerWidth;
	newRect[kPsychBottom]=newRect[kPsychTop]+innerHeight;
}

/*
    PsychGetCenterFromRectAbs()
    
    For consistency in how we treat sides of an even number of pixels we should always use
    PsychGetCenterFromRect() when finding the center of a rect. 
    
    We return the result in absolute coordinates i.e. we preserve offsets from the origin
*/
void PsychGetCenterFromRectAbsolute(const double *rect, double *rectCenterX, double *rectCenterY)
{
        double rectWidth, rectHeight;
        rectWidth=PsychGetWidthFromRect(rect);
        rectHeight=PsychGetHeightFromRect(rect);
        *rectCenterX=rect[kPsychLeft] + (rectWidth/2);
        *rectCenterY=rect[kPsychTop] + (rectHeight/2);
}


void PsychCopyRect(double *toRect, const double *fromRect)
{
    memcpy(toRect, fromRect, sizeof(PsychRectType));
}


/*
    PsychGetManhattanDistance()
    
    Find the offset between two points.  The distance to travel from point 1 to point 2. 
    
    The caller allocated deltaX and deltaY.  
*/
void PsychGetManhattanDistance(double x1, double y1, double x2, double y2, double *deltaX, double *deltaY)
{
    *deltaX=x2-x1;
    *deltaY=y2-y1;
}

/*
	PsychNormalizeRect()
	
	Offsets rect to the origin, making left and top coordinates 0 and 0.
*/
void PsychNormalizeRect(double *rect, double *normRect)
{
	
	//x
	normRect[kPsychLeft]=(double)0;								//normRect[kPsychLeft]=rect[kPsychLeft]-rect[kPsychLeft];
	normRect[kPsychRight]=fabs(rect[kPsychRight]-rect[kPsychLeft]);
	//y
	normRect[kPsychTop]=(double)0;								//normRect[kPsychTop]=rect[kPsychTop]-rect[kPsychTop];
	normRect[kPsychBottom]=fabs(rect[kPsychBottom]-rect[kPsychTop]);
}


/*
	PsychInverRectYInFrame()
	
	Convert the rect for and coordinate frame with an inverted Y axis.  
	
	Calculate rectB given rectA within frameA.  If rectA lies within coordinate frame frameA where the origin is at bottom left corner of the display, then 
	rectB will be on the same screen position within coordinate frame frameB where the origin lies at the top left.
	
	
*/
void PsychInvertRectY(double *rectB, double *rectA, double *frameA)
{
	double  frameHeight, offset;
	
	PsychCopyRect(rectB, rectA);
	frameHeight=PsychGetHeightFromRect(frameA);
	offset=frameHeight-PsychGetHeightFromRect(rectA);
	rectB[kPsychBottom]=frameHeight - rectA[kPsychTop] - offset;
	rectB[kPsychTop]=frameHeight - rectA[kPsychBottom] - offset;
	
}

psych_bool PsychMatchRect(double *rectA, double *RectB)
{
	return( rectA[kPsychLeft] == RectB[kPsychLeft] &&
			rectA[kPsychTop] == RectB[kPsychTop] &&
			rectA[kPsychRight] == RectB[kPsychRight] &&
			rectA[kPsychBottom] == RectB[kPsychBottom]);

}

/* 
	PsychFindEnclosingTextureRect

	Except when using the GL_EXT_texture_rectangle, OpenGL textures must be have sides which are a power of two. 
	Therefore, when we want to hold within a texture a graphic which does not meet that requirement,  we  find the
	smallest bounding rectangle with sides length 2^n still large enough to contain the graphic.
	
	The caller allocates enclosingRect.
*/

void PsychFindEnclosingTextureRect(double *rectA, double *enclosingRect)
{
	double			rectSides[2];
	unsigned long	newSides[2];
	int				s;
 			
	rectSides[0]=PsychGetWidthFromRect(rectA);
	rectSides[1]=PsychGetHeightFromRect(rectA);
	

	for(s=0;s<2;s++){
		for(newSides[s]=1;newSides[s]<rectSides[s];newSides[s]=newSides[s]<<1);
	}
	enclosingRect[kPsychLeft]=0;
	enclosingRect[kPsychTop]=0;
	enclosingRect[kPsychRight]=(double)newSides[0];
	enclosingRect[kPsychBottom]=(double)newSides[1];
	
}

