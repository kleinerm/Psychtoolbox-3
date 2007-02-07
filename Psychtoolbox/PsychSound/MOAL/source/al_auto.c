
/*
 * al_auto.c
 *
 * 06-Feb-2007 -- created (moglgen)
 *
 */

#include "moaltypes.h"

void al_enable( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == alEnable) mogl_glunsupported("alEnable");
	alEnable((ALenum)mxGetScalar(prhs[0]));

}

void al_disable( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == alDisable) mogl_glunsupported("alDisable");
	alDisable((ALenum)mxGetScalar(prhs[0]));

}

void al_isenabled( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == alIsEnabled) mogl_glunsupported("alIsEnabled");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)alIsEnabled((ALenum)mxGetScalar(prhs[0]));

}

void al_getbooleanv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == alGetBooleanv) mogl_glunsupported("alGetBooleanv");
	alGetBooleanv((ALenum)mxGetScalar(prhs[0]),
		(ALboolean*)mxGetData(prhs[1]));

}

void al_getintegerv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == alGetIntegerv) mogl_glunsupported("alGetIntegerv");
	alGetIntegerv((ALenum)mxGetScalar(prhs[0]),
		(ALint*)mxGetData(prhs[1]));

}

void al_getfloatv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == alGetFloatv) mogl_glunsupported("alGetFloatv");
	alGetFloatv((ALenum)mxGetScalar(prhs[0]),
		(ALfloat*)mxGetData(prhs[1]));

}

void al_getdoublev( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == alGetDoublev) mogl_glunsupported("alGetDoublev");
	alGetDoublev((ALenum)mxGetScalar(prhs[0]),
		(ALdouble*)mxGetData(prhs[1]));

}

void al_getboolean( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == alGetBoolean) mogl_glunsupported("alGetBoolean");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)alGetBoolean((ALenum)mxGetScalar(prhs[0]));

}

void al_getinteger( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == alGetInteger) mogl_glunsupported("alGetInteger");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)alGetInteger((ALenum)mxGetScalar(prhs[0]));

}

void al_getfloat( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == alGetFloat) mogl_glunsupported("alGetFloat");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)alGetFloat((ALenum)mxGetScalar(prhs[0]));

}

void al_getdouble( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == alGetDouble) mogl_glunsupported("alGetDouble");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)alGetDouble((ALenum)mxGetScalar(prhs[0]));

}

void al_geterror( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == alGetError) mogl_glunsupported("alGetError");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)alGetError();

}

void al_isextensionpresent( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == alIsExtensionPresent) mogl_glunsupported("alIsExtensionPresent");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)alIsExtensionPresent((const ALchar*)mxGetData(prhs[0]));

}

void al_getenumvalue( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == alGetEnumValue) mogl_glunsupported("alGetEnumValue");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)alGetEnumValue((const ALchar*)mxGetData(prhs[0]));

}

void al_listenerf( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == alListenerf) mogl_glunsupported("alListenerf");
	alListenerf((ALenum)mxGetScalar(prhs[0]),
		(ALfloat)mxGetScalar(prhs[1]));

}

void al_listener3f( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == alListener3f) mogl_glunsupported("alListener3f");
	alListener3f((ALenum)mxGetScalar(prhs[0]),
		(ALfloat)mxGetScalar(prhs[1]),
		(ALfloat)mxGetScalar(prhs[2]),
		(ALfloat)mxGetScalar(prhs[3]));

}

void al_listenerfv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == alListenerfv) mogl_glunsupported("alListenerfv");
	alListenerfv((ALenum)mxGetScalar(prhs[0]),
		(const ALfloat*)mxGetData(prhs[1]));

}

void al_listeneri( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == alListeneri) mogl_glunsupported("alListeneri");
	alListeneri((ALenum)mxGetScalar(prhs[0]),
		(ALint)mxGetScalar(prhs[1]));

}

void al_listener3i( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == alListener3i) mogl_glunsupported("alListener3i");
	alListener3i((ALenum)mxGetScalar(prhs[0]),
		(ALint)mxGetScalar(prhs[1]),
		(ALint)mxGetScalar(prhs[2]),
		(ALint)mxGetScalar(prhs[3]));

}

void al_listeneriv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == alListeneriv) mogl_glunsupported("alListeneriv");
	alListeneriv((ALenum)mxGetScalar(prhs[0]),
		(const ALint*)mxGetData(prhs[1]));

}

void al_getlistenerf( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == alGetListenerf) mogl_glunsupported("alGetListenerf");
	alGetListenerf((ALenum)mxGetScalar(prhs[0]),
		(ALfloat*)mxGetData(prhs[1]));

}

void al_getlistener3f( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == alGetListener3f) mogl_glunsupported("alGetListener3f");
	alGetListener3f((ALenum)mxGetScalar(prhs[0]),
		(ALfloat*)mxGetData(prhs[1]),
		(ALfloat*)mxGetData(prhs[2]),
		(ALfloat*)mxGetData(prhs[3]));

}

void al_getlistenerfv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == alGetListenerfv) mogl_glunsupported("alGetListenerfv");
	alGetListenerfv((ALenum)mxGetScalar(prhs[0]),
		(ALfloat*)mxGetData(prhs[1]));

}

void al_getlisteneri( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == alGetListeneri) mogl_glunsupported("alGetListeneri");
	alGetListeneri((ALenum)mxGetScalar(prhs[0]),
		(ALint*)mxGetData(prhs[1]));

}

void al_getlistener3i( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == alGetListener3i) mogl_glunsupported("alGetListener3i");
	alGetListener3i((ALenum)mxGetScalar(prhs[0]),
		(ALint*)mxGetData(prhs[1]),
		(ALint*)mxGetData(prhs[2]),
		(ALint*)mxGetData(prhs[3]));

}

void al_getlisteneriv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == alGetListeneriv) mogl_glunsupported("alGetListeneriv");
	alGetListeneriv((ALenum)mxGetScalar(prhs[0]),
		(ALint*)mxGetData(prhs[1]));

}

void al_gensources( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == alGenSources) mogl_glunsupported("alGenSources");
	alGenSources((ALsizei)mxGetScalar(prhs[0]),
		(ALuint*)mxGetData(prhs[1]));

}

void al_deletesources( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == alDeleteSources) mogl_glunsupported("alDeleteSources");
	alDeleteSources((ALsizei)mxGetScalar(prhs[0]),
		(const ALuint*)mxGetData(prhs[1]));

}

void al_issource( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == alIsSource) mogl_glunsupported("alIsSource");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)alIsSource((ALuint)mxGetScalar(prhs[0]));

}

void al_sourcef( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == alSourcef) mogl_glunsupported("alSourcef");
	alSourcef((ALuint)mxGetScalar(prhs[0]),
		(ALenum)mxGetScalar(prhs[1]),
		(ALfloat)mxGetScalar(prhs[2]));

}

void al_source3f( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == alSource3f) mogl_glunsupported("alSource3f");
	alSource3f((ALuint)mxGetScalar(prhs[0]),
		(ALenum)mxGetScalar(prhs[1]),
		(ALfloat)mxGetScalar(prhs[2]),
		(ALfloat)mxGetScalar(prhs[3]),
		(ALfloat)mxGetScalar(prhs[4]));

}

void al_sourcefv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == alSourcefv) mogl_glunsupported("alSourcefv");
	alSourcefv((ALuint)mxGetScalar(prhs[0]),
		(ALenum)mxGetScalar(prhs[1]),
		(const ALfloat*)mxGetData(prhs[2]));

}

void al_sourcei( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == alSourcei) mogl_glunsupported("alSourcei");
	alSourcei((ALuint)mxGetScalar(prhs[0]),
		(ALenum)mxGetScalar(prhs[1]),
		(ALint)mxGetScalar(prhs[2]));

}

void al_source3i( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == alSource3i) mogl_glunsupported("alSource3i");
	alSource3i((ALuint)mxGetScalar(prhs[0]),
		(ALenum)mxGetScalar(prhs[1]),
		(ALint)mxGetScalar(prhs[2]),
		(ALint)mxGetScalar(prhs[3]),
		(ALint)mxGetScalar(prhs[4]));

}

void al_sourceiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == alSourceiv) mogl_glunsupported("alSourceiv");
	alSourceiv((ALuint)mxGetScalar(prhs[0]),
		(ALenum)mxGetScalar(prhs[1]),
		(const ALint*)mxGetData(prhs[2]));

}

void al_getsourcef( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == alGetSourcef) mogl_glunsupported("alGetSourcef");
	alGetSourcef((ALuint)mxGetScalar(prhs[0]),
		(ALenum)mxGetScalar(prhs[1]),
		(ALfloat*)mxGetData(prhs[2]));

}

void al_getsource3f( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == alGetSource3f) mogl_glunsupported("alGetSource3f");
	alGetSource3f((ALuint)mxGetScalar(prhs[0]),
		(ALenum)mxGetScalar(prhs[1]),
		(ALfloat*)mxGetData(prhs[2]),
		(ALfloat*)mxGetData(prhs[3]),
		(ALfloat*)mxGetData(prhs[4]));

}

void al_getsourcefv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == alGetSourcefv) mogl_glunsupported("alGetSourcefv");
	alGetSourcefv((ALuint)mxGetScalar(prhs[0]),
		(ALenum)mxGetScalar(prhs[1]),
		(ALfloat*)mxGetData(prhs[2]));

}

void al_getsourcei( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == alGetSourcei) mogl_glunsupported("alGetSourcei");
	alGetSourcei((ALuint)mxGetScalar(prhs[0]),
		(ALenum)mxGetScalar(prhs[1]),
		(ALint*)mxGetData(prhs[2]));

}

void al_getsource3i( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == alGetSource3i) mogl_glunsupported("alGetSource3i");
	alGetSource3i((ALuint)mxGetScalar(prhs[0]),
		(ALenum)mxGetScalar(prhs[1]),
		(ALint*)mxGetData(prhs[2]),
		(ALint*)mxGetData(prhs[3]),
		(ALint*)mxGetData(prhs[4]));

}

void al_getsourceiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == alGetSourceiv) mogl_glunsupported("alGetSourceiv");
	alGetSourceiv((ALuint)mxGetScalar(prhs[0]),
		(ALenum)mxGetScalar(prhs[1]),
		(ALint*)mxGetData(prhs[2]));

}

void al_sourceplayv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == alSourcePlayv) mogl_glunsupported("alSourcePlayv");
	alSourcePlayv((ALsizei)mxGetScalar(prhs[0]),
		(const ALuint*)mxGetData(prhs[1]));

}

void al_sourcestopv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == alSourceStopv) mogl_glunsupported("alSourceStopv");
	alSourceStopv((ALsizei)mxGetScalar(prhs[0]),
		(const ALuint*)mxGetData(prhs[1]));

}

void al_sourcerewindv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == alSourceRewindv) mogl_glunsupported("alSourceRewindv");
	alSourceRewindv((ALsizei)mxGetScalar(prhs[0]),
		(const ALuint*)mxGetData(prhs[1]));

}

void al_sourcepausev( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == alSourcePausev) mogl_glunsupported("alSourcePausev");
	alSourcePausev((ALsizei)mxGetScalar(prhs[0]),
		(const ALuint*)mxGetData(prhs[1]));

}

void al_sourceplay( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == alSourcePlay) mogl_glunsupported("alSourcePlay");
	alSourcePlay((ALuint)mxGetScalar(prhs[0]));

}

void al_sourcestop( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == alSourceStop) mogl_glunsupported("alSourceStop");
	alSourceStop((ALuint)mxGetScalar(prhs[0]));

}

void al_sourcerewind( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == alSourceRewind) mogl_glunsupported("alSourceRewind");
	alSourceRewind((ALuint)mxGetScalar(prhs[0]));

}

void al_sourcepause( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == alSourcePause) mogl_glunsupported("alSourcePause");
	alSourcePause((ALuint)mxGetScalar(prhs[0]));

}

void al_sourcequeuebuffers( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == alSourceQueueBuffers) mogl_glunsupported("alSourceQueueBuffers");
	alSourceQueueBuffers((ALuint)mxGetScalar(prhs[0]),
		(ALsizei)mxGetScalar(prhs[1]),
		(const ALuint*)mxGetData(prhs[2]));

}

void al_sourceunqueuebuffers( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == alSourceUnqueueBuffers) mogl_glunsupported("alSourceUnqueueBuffers");
	alSourceUnqueueBuffers((ALuint)mxGetScalar(prhs[0]),
		(ALsizei)mxGetScalar(prhs[1]),
		(ALuint*)mxGetData(prhs[2]));

}

void al_genbuffers( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == alGenBuffers) mogl_glunsupported("alGenBuffers");
	alGenBuffers((ALsizei)mxGetScalar(prhs[0]),
		(ALuint*)mxGetData(prhs[1]));

}

void al_deletebuffers( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == alDeleteBuffers) mogl_glunsupported("alDeleteBuffers");
	alDeleteBuffers((ALsizei)mxGetScalar(prhs[0]),
		(const ALuint*)mxGetData(prhs[1]));

}

void al_isbuffer( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == alIsBuffer) mogl_glunsupported("alIsBuffer");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)alIsBuffer((ALuint)mxGetScalar(prhs[0]));

}

void al_bufferdata( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == alBufferData) mogl_glunsupported("alBufferData");
	alBufferData((ALuint)mxGetScalar(prhs[0]),
		(ALenum)mxGetScalar(prhs[1]),
		(const ALvoid*)mxGetData(prhs[2]),
		(ALsizei)mxGetScalar(prhs[3]),
		(ALsizei)mxGetScalar(prhs[4]));

}

void al_bufferf( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == alBufferf) mogl_glunsupported("alBufferf");
	alBufferf((ALuint)mxGetScalar(prhs[0]),
		(ALenum)mxGetScalar(prhs[1]),
		(ALfloat)mxGetScalar(prhs[2]));

}

void al_buffer3f( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == alBuffer3f) mogl_glunsupported("alBuffer3f");
	alBuffer3f((ALuint)mxGetScalar(prhs[0]),
		(ALenum)mxGetScalar(prhs[1]),
		(ALfloat)mxGetScalar(prhs[2]),
		(ALfloat)mxGetScalar(prhs[3]),
		(ALfloat)mxGetScalar(prhs[4]));

}

void al_bufferfv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == alBufferfv) mogl_glunsupported("alBufferfv");
	alBufferfv((ALuint)mxGetScalar(prhs[0]),
		(ALenum)mxGetScalar(prhs[1]),
		(const ALfloat*)mxGetData(prhs[2]));

}

void al_bufferi( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == alBufferi) mogl_glunsupported("alBufferi");
	alBufferi((ALuint)mxGetScalar(prhs[0]),
		(ALenum)mxGetScalar(prhs[1]),
		(ALint)mxGetScalar(prhs[2]));

}

void al_buffer3i( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == alBuffer3i) mogl_glunsupported("alBuffer3i");
	alBuffer3i((ALuint)mxGetScalar(prhs[0]),
		(ALenum)mxGetScalar(prhs[1]),
		(ALint)mxGetScalar(prhs[2]),
		(ALint)mxGetScalar(prhs[3]),
		(ALint)mxGetScalar(prhs[4]));

}

void al_bufferiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == alBufferiv) mogl_glunsupported("alBufferiv");
	alBufferiv((ALuint)mxGetScalar(prhs[0]),
		(ALenum)mxGetScalar(prhs[1]),
		(const ALint*)mxGetData(prhs[2]));

}

void al_getbufferf( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == alGetBufferf) mogl_glunsupported("alGetBufferf");
	alGetBufferf((ALuint)mxGetScalar(prhs[0]),
		(ALenum)mxGetScalar(prhs[1]),
		(ALfloat*)mxGetData(prhs[2]));

}

void al_getbuffer3f( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == alGetBuffer3f) mogl_glunsupported("alGetBuffer3f");
	alGetBuffer3f((ALuint)mxGetScalar(prhs[0]),
		(ALenum)mxGetScalar(prhs[1]),
		(ALfloat*)mxGetData(prhs[2]),
		(ALfloat*)mxGetData(prhs[3]),
		(ALfloat*)mxGetData(prhs[4]));

}

void al_getbufferfv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == alGetBufferfv) mogl_glunsupported("alGetBufferfv");
	alGetBufferfv((ALuint)mxGetScalar(prhs[0]),
		(ALenum)mxGetScalar(prhs[1]),
		(ALfloat*)mxGetData(prhs[2]));

}

void al_getbufferi( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == alGetBufferi) mogl_glunsupported("alGetBufferi");
	alGetBufferi((ALuint)mxGetScalar(prhs[0]),
		(ALenum)mxGetScalar(prhs[1]),
		(ALint*)mxGetData(prhs[2]));

}

void al_getbuffer3i( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == alGetBuffer3i) mogl_glunsupported("alGetBuffer3i");
	alGetBuffer3i((ALuint)mxGetScalar(prhs[0]),
		(ALenum)mxGetScalar(prhs[1]),
		(ALint*)mxGetData(prhs[2]),
		(ALint*)mxGetData(prhs[3]),
		(ALint*)mxGetData(prhs[4]));

}

void al_getbufferiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == alGetBufferiv) mogl_glunsupported("alGetBufferiv");
	alGetBufferiv((ALuint)mxGetScalar(prhs[0]),
		(ALenum)mxGetScalar(prhs[1]),
		(ALint*)mxGetData(prhs[2]));

}

void al_dopplerfactor( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == alDopplerFactor) mogl_glunsupported("alDopplerFactor");
	alDopplerFactor((ALfloat)mxGetScalar(prhs[0]));

}

void al_dopplervelocity( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == alDopplerVelocity) mogl_glunsupported("alDopplerVelocity");
	alDopplerVelocity((ALfloat)mxGetScalar(prhs[0]));

}

void al_speedofsound( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == alSpeedOfSound) mogl_glunsupported("alSpeedOfSound");
	alSpeedOfSound((ALfloat)mxGetScalar(prhs[0]));

}

void al_distancemodel( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == alDistanceModel) mogl_glunsupported("alDistanceModel");
	alDistanceModel((ALenum)mxGetScalar(prhs[0]));

}

int gl_auto_map_count=71;
cmdhandler gl_auto_map[] = {
{ "alBuffer3f",                      al_buffer3f                         },
{ "alBuffer3i",                      al_buffer3i                         },
{ "alBufferData",                    al_bufferdata                       },
{ "alBufferf",                       al_bufferf                          },
{ "alBufferfv",                      al_bufferfv                         },
{ "alBufferi",                       al_bufferi                          },
{ "alBufferiv",                      al_bufferiv                         },
{ "alDeleteBuffers",                 al_deletebuffers                    },
{ "alDeleteSources",                 al_deletesources                    },
{ "alDisable",                       al_disable                          },
{ "alDistanceModel",                 al_distancemodel                    },
{ "alDopplerFactor",                 al_dopplerfactor                    },
{ "alDopplerVelocity",               al_dopplervelocity                  },
{ "alEnable",                        al_enable                           },
{ "alGenBuffers",                    al_genbuffers                       },
{ "alGenSources",                    al_gensources                       },
{ "alGetBoolean",                    al_getboolean                       },
{ "alGetBooleanv",                   al_getbooleanv                      },
{ "alGetBuffer3f",                   al_getbuffer3f                      },
{ "alGetBuffer3i",                   al_getbuffer3i                      },
{ "alGetBufferf",                    al_getbufferf                       },
{ "alGetBufferfv",                   al_getbufferfv                      },
{ "alGetBufferi",                    al_getbufferi                       },
{ "alGetBufferiv",                   al_getbufferiv                      },
{ "alGetDouble",                     al_getdouble                        },
{ "alGetDoublev",                    al_getdoublev                       },
{ "alGetEnumValue",                  al_getenumvalue                     },
{ "alGetError",                      al_geterror                         },
{ "alGetFloat",                      al_getfloat                         },
{ "alGetFloatv",                     al_getfloatv                        },
{ "alGetInteger",                    al_getinteger                       },
{ "alGetIntegerv",                   al_getintegerv                      },
{ "alGetListener3f",                 al_getlistener3f                    },
{ "alGetListener3i",                 al_getlistener3i                    },
{ "alGetListenerf",                  al_getlistenerf                     },
{ "alGetListenerfv",                 al_getlistenerfv                    },
{ "alGetListeneri",                  al_getlisteneri                     },
{ "alGetListeneriv",                 al_getlisteneriv                    },
{ "alGetSource3f",                   al_getsource3f                      },
{ "alGetSource3i",                   al_getsource3i                      },
{ "alGetSourcef",                    al_getsourcef                       },
{ "alGetSourcefv",                   al_getsourcefv                      },
{ "alGetSourcei",                    al_getsourcei                       },
{ "alGetSourceiv",                   al_getsourceiv                      },
{ "alIsBuffer",                      al_isbuffer                         },
{ "alIsEnabled",                     al_isenabled                        },
{ "alIsExtensionPresent",            al_isextensionpresent               },
{ "alIsSource",                      al_issource                         },
{ "alListener3f",                    al_listener3f                       },
{ "alListener3i",                    al_listener3i                       },
{ "alListenerf",                     al_listenerf                        },
{ "alListenerfv",                    al_listenerfv                       },
{ "alListeneri",                     al_listeneri                        },
{ "alListeneriv",                    al_listeneriv                       },
{ "alSource3f",                      al_source3f                         },
{ "alSource3i",                      al_source3i                         },
{ "alSourcePause",                   al_sourcepause                      },
{ "alSourcePausev",                  al_sourcepausev                     },
{ "alSourcePlay",                    al_sourceplay                       },
{ "alSourcePlayv",                   al_sourceplayv                      },
{ "alSourceQueueBuffers",            al_sourcequeuebuffers               },
{ "alSourceRewind",                  al_sourcerewind                     },
{ "alSourceRewindv",                 al_sourcerewindv                    },
{ "alSourceStop",                    al_sourcestop                       },
{ "alSourceStopv",                   al_sourcestopv                      },
{ "alSourceUnqueueBuffers",          al_sourceunqueuebuffers             },
{ "alSourcef",                       al_sourcef                          },
{ "alSourcefv",                      al_sourcefv                         },
{ "alSourcei",                       al_sourcei                          },
{ "alSourceiv",                      al_sourceiv                         },
{ "alSpeedOfSound",                  al_speedofsound                     }};
