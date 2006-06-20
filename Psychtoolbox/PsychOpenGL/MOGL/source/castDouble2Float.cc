#include <octave/oct.h>

DEFUN_DLD (castDouble2Float, args, ,
"Helper routine for MOGL on Octave:\n"
"Converts double value objects into\n"
"objects of type uint32 which internally\n"
"store 32 bit single precision float values.\n"
"-> Convert double into float and wrap the float\n"
"into a uint32 object.\n\n"
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

  // Create output uint8 array of matching dimensionality:
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
