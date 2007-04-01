#include <octave/oct.h>

DEFUN_DLD (castDouble2Float, args, ,
"Helper routine for MOGL on Octave:\n"
"out = castDouble2Float(invalue [direction=0])\n"
"direction = 0 ==> out = single(invalue) packed in uint32.\n"
"direction = 1 ==> out = double(invalue) which is a float packed int uint32.\n"
"Converts double value objects into\n"
"objects of type uint32 which internally\n"
"store 32 bit single precision float values.\n"
"-> Convert double into float and wrap the float\n"
"into a uint32 object. Does also the opposite direction.\n\n"
"This is a neccessary hack used by our single.m\n"
"file to create a pseudo single data type that\n"
"can be fed into moglcore when it expects float\n"
"values. This is a poor replacement for a real\n"
"single precision Octave datatype, as it involves\n"
"an expensive double->float cast and copy op and\n"
"call overhead. Also, this single() datatype is\n"
"completely inaccessible by Octave...\n\n")
{
  if (args.length()<1) {
    error("castDouble2Float: You need to specify a double object as 1st argument!");
    return(octave_value());
  }

  if (!args(0).is_real_type()) {
    error("castDouble2Float: You need to specify a double object as 1st argument!");
    return(octave_value());
  }

  // Retrieve dimensionality of input argument:
  dim_vector indims = args(0).dims();

  // And number of elements:
  int count = args(0).numel();

  if (args.length()>=2 && args(1).double_value()==1) {
    // Float-->Double cast: Input is float's, packed into uint32 array,
    // output shall be a double representation:

    // Create output double array of matching dimensionality:
    NDArray mout(indims);

    // Retrieve a double-pointer to internal representation. As mout is new
    // this won't trigger a deep-copy.
    double* outptr = (double*) mout.data();
    
    // Retrieve a uint32-pointer to internal representation of input array:
    const uint32NDArray matIn(args(0).array_value());
    float* inptr = (float*) matIn.data();

    // Cast & Copy loop:
    for(int i=0; i<count; i++) *(outptr++) = (double)(*(inptr++));

    // Assign mout as our output octave-value:
    return(octave_value(mout));
  }
  else {
    // Double-->Float cast: Input is double, output is float, packed
    // into a uint32 array:

    // Create output uint32 array of matching dimensionality:
    uint32NDArray mout(indims);

    // Retrieve a float-pointer to internal representation. As mout is new
    // this won't trigger a deep-copy.
    float* outptr = (float*) mout.data();
    
    // Retrieve a double-pointer to internal representation of input array:
    const NDArray matIn(args(0).array_value());
    double* inptr = (double*) matIn.data();

    // Cast & Copy loop:
    for(int i=0; i<count; i++) *(outptr++) = (float)(*(inptr++));

    // Assign mout as our output octave-value:
    return(octave_value(mout));
  }
}
