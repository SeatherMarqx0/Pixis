#include "Pixis_JNI.h"
#include <stdio.h>
#include <stdlib.h>
#include <exception>
#include "master.h"
#include "pvcam.h"
#include "Pixis.h"
#include <iostream>
#include <ctime>
#include <string>
#include <sstream>
#include <iomanip>
#include <vector>

using namespace std;

/*
 * Class:     cxro_common_device_Pixis_Pixis
 * Method:    Pixis_Init
 * Signature: ([JI)Z
 */
JNIEXPORT jboolean JNICALL Java_cxro_common_device_Pixis_Pixis_Pixis_1Init
(JNIEnv *env, jobject obj, jlongArray handle, jint number)
{
    //Create object instance
    Pixis *cam;
    try
    {
	    cam = new Pixis((short)number);
    }
    catch(exception *e)
    {
        return false;
    }
	//Store into passed handle
	jlong tmpH[1] = {(long)cam};
	env->SetLongArrayRegion(handle,0,1,tmpH);
	return true;
}

/*
* Class:     cxro_common_device_Pixis
* Method:    Pixis_Uninit
* Signature: ([J)Z
*/
JNIEXPORT jboolean JNICALL Java_cxro_common_device_Pixis_Pixis_Pixis_1Uninit
(JNIEnv *env, jobject obj, jlongArray handle)
{
    jlong *tmpH = env->GetLongArrayElements(handle,0);
	Pixis *cam = reinterpret_cast<Pixis*>(*tmpH);
	env->ReleaseLongArrayElements(handle,tmpH,1);
	cam->~Pixis();
	//handle[0] = 0;
	return true;
}

/*
* Class:     cxro_common_device_Pixis
* Method:    get_sensor_size
* Signature: ([J[C)Z
*/
JNIEXPORT jboolean JNICALL Java_cxro_common_device_Pixis_Pixis_get_1sensor_1size
(JNIEnv *env, jobject obj, jlong handle, jcharArray size)
{
    Pixis* cam = reinterpret_cast<Pixis*>(handle);
    jchar elems[2];
    jboolean result = cam->get_sensor_size(&elems[0],&elems[1]);
    env->SetCharArrayRegion(size,0,2,elems);

    //env->ReleaseCharArrayElements(size,elems,0);
    return result;
}

/*
* Class:     cxro_common_device_Pixis
* Method:    exp_setup
* Signature: ([JIIIIIII[J)Z
*/
JNIEXPORT jboolean JNICALL Java_cxro_common_device_Pixis_Pixis_exp_1setup
(JNIEnv *env, jobject obj, jlong handle, jint exposure_time, jint x0, jint x1, jint y0, jint y1, jint xbin, jint ybin, jlongArray stream_size)
{
    Pixis* cam = reinterpret_cast<Pixis*>(handle);
    unsigned long tmpS = 0;
    jboolean result = cam->exp_setup(exposure_time, (unsigned short)x0, (unsigned short)x1, (unsigned short)y0, (unsigned short)y1, (unsigned short)xbin, (unsigned short)ybin, &tmpS);
	env->SetLongArrayRegion(stream_size,0,1,reinterpret_cast<const jlong*>(&tmpS));
    return result;
}

/*
* Class:     cxro_common_device_Pixis
* Method:    exp_start
* Signature: ([JLjava/nio/ByteBuffer{})Z
*/
JNIEXPORT jboolean JNICALL Java_cxro_common_device_Pixis_Pixis_exp_1start
(JNIEnv *env, jobject obj, jlong handle, jobject image)
{
    Pixis* cam = reinterpret_cast<Pixis*>(handle);
    unsigned short* imgBuff = (unsigned short*) env->GetDirectBufferAddress(image);
    jboolean result = cam->exp_start(imgBuff);
    return result;
}

/*
* Class:     cxro_common_device_Pixis
* Method:    exp_check_status
* Signature: ([J[I)Z
*/
JNIEXPORT jboolean JNICALL Java_cxro_common_device_Pixis_Pixis_exp_1check_1status
(JNIEnv *env, jobject obj, jlong handle, jintArray status)
{
    Pixis* cam = reinterpret_cast<Pixis*>(handle);
    jint *elems = env->GetIntArrayElements(status,0);
    elems[0] = 0;
    jboolean result = cam->exp_check_status(reinterpret_cast<short*>(&elems[0]),reinterpret_cast<unsigned long*>(&elems[1]));
    env->ReleaseIntArrayElements(status,elems,0);
    return result;
}

/*
* Class:     cxro_common_device_Pixis
* Method:    exp_halt
* Signature: ([JS)Z
*/
JNIEXPORT jboolean JNICALL Java_cxro_common_device_Pixis_Pixis_exp_1halt
(JNIEnv *env, jobject obj, jlong handle, jshort cam_state)
{
    Pixis* cam = reinterpret_cast<Pixis*>(handle);
    jboolean result = cam->exp_halt(cam_state);
    return result;
}

/*
* Class:     cxro_common_device_Pixis
* Method:    cont_start
* Signature: ([JIIIIIII[J)Z
*/
JNIEXPORT jboolean JNICALL Java_cxro_common_device_Pixis_Pixis_cont_1start
(JNIEnv *env, jobject obj, jlong handle, jint exposure_time, jint x0, jint x1, jint y0, jint y1, jint xbin, jint ybin, jlongArray stream_size)
{
    Pixis* cam = reinterpret_cast<Pixis*>(handle);
    unsigned long tmpS;
    jboolean result = cam->cont_start(exposure_time, (unsigned short)x0, (unsigned short)x1, (unsigned short)y0, (unsigned short)y1, (unsigned short)xbin, (unsigned short)ybin, &tmpS);
	env->SetLongArrayRegion(stream_size,0,1,reinterpret_cast<const jlong*>(&tmpS));
    return result;
}

/*
* Class:     cxro_common_device_Pixis
* Method:    cont_check_status
* Signature: ([J[I)Z
*/
JNIEXPORT jboolean JNICALL Java_cxro_common_device_Pixis_Pixis_cont_1check_1status
(JNIEnv *env, jobject obj, jlong handle, jintArray status)
{
    Pixis* cam = reinterpret_cast<Pixis*>(handle);
    jint elems[] = {0,0,0};
    jboolean result = cam->cont_check_status(reinterpret_cast<short*>(&elems[0]),reinterpret_cast<unsigned long*>(&elems[1]),reinterpret_cast<unsigned long*>(&elems[2]));
    env->SetIntArrayRegion(status,0,3,elems);
    return result;
}

/*
* Class:     cxro_common_device_Pixis
* Method:    cont_halt
* Signature: ([JS)Z
*/
JNIEXPORT jboolean JNICALL Java_cxro_common_device_Pixis_Pixis_cont_1halt
(JNIEnv *env, jobject obj, jlong handle, jshort cam_state)
{
    Pixis* cam = reinterpret_cast<Pixis*>(handle);
    jboolean result = cam->cont_halt(cam_state);
    return result;
}

/*
* Class:     cxro_common_device_Pixis
* Method:    cont_get_frame
* Signature: ([JLjava/nio/ByteBuffer{})Z
*/
JNIEXPORT jboolean JNICALL Java_cxro_common_device_Pixis_Pixis_cont_1get_1frame
(JNIEnv *env, jobject obj, jlong handle, jobject image)
{
    Pixis* cam = reinterpret_cast<Pixis*>(handle);
    unsigned short* imgBuff = (unsigned short*) env->GetDirectBufferAddress(image);
    jboolean result = cam->cont_get_frame(imgBuff);
    return result;
}

/*
* Class:     cxro_common_device_Pixis
* Method:    get_error
* Signature: ([J)Ljava/lang/String{}
*/
JNIEXPORT jstring JNICALL Java_cxro_common_device_Pixis_Pixis_get_1error
(JNIEnv *env, jobject obj, jlong handle)
{
    Pixis* cam = reinterpret_cast<Pixis*>(handle);
    std::string err;
    jboolean result = cam->get_error(err);
    jstring answer = env->NewStringUTF(err.c_str());
    return answer;
}

/*
* Class:     cxro_common_device_Pixis
* Method:    get_paramList
* Signature: ([J)[Ljava/lang/String{}
*/
JNIEXPORT jobjectArray JNICALL Java_cxro_common_device_Pixis_Pixis_get_1paramList
(JNIEnv *env, jobject obj, jlong handle)
{
    Pixis* cam = reinterpret_cast<Pixis*>(handle);
    jobjectArray ret;
    std::vector<std::string> params;
    short num;
    bool status = cam->get_paramList(params, num);
    ret= (jobjectArray)env->NewObjectArray(params.size(),  
        env->FindClass("java/lang/String"),  
        env->NewStringUTF(""));

    for(int i=0;i < params.size();i++)
    {  
        env->SetObjectArrayElement(ret,i,env->NewStringUTF(params[i].c_str()));
    }
    return ret;
}

/*
 * Class:     cxro_common_device_Pixis_Pixis
 * Method:    get_ccs_status
 * Signature: (J[I)Z
 */
JNIEXPORT jboolean JNICALL Java_cxro_common_device_Pixis_Pixis_get_1ccs_1status
(JNIEnv *env, jobject obj, jlong handle, jintArray status)
{
    Pixis* cam = reinterpret_cast<Pixis*>(handle);
    unsigned short ccs_status;
    jboolean result = cam->get_ccs_status(&ccs_status);
    env->SetIntArrayRegion(status,0,1,reinterpret_cast<const jint*>(&ccs_status));
    return result;
}

/*
 * Class:     cxro_common_device_Pixis_Pixis
 * Method:    get_shtr_status
 * Signature: (J[S)Z
 */
JNIEXPORT jboolean JNICALL Java_cxro_common_device_Pixis_Pixis_get_1shtr_1status
(JNIEnv *env, jobject obj, jlong handle, jshortArray status)
{
    Pixis* cam = reinterpret_cast<Pixis*>(handle);
    short shtr_status;
    jboolean result = cam->get_shtr_status(&shtr_status);
    env->SetShortArrayRegion(status,0,1,const_cast<const jshort*>(&shtr_status));
    return result;
}

/*
* Class:     cxro_common_device_Pixis
* Method:    shtr_open_never
* Signature: ([J)Z
*/
JNIEXPORT jboolean JNICALL Java_cxro_common_device_Pixis_Pixis_shtr_1open_1never
(JNIEnv *env, jobject obj, jlong handle)
{
    Pixis* cam = reinterpret_cast<Pixis*>(handle);
    return cam->shtr_open_never();
}

/*
* Class:     cxro_common_device_Pixis
* Method:    shtr_open_normal
* Signature: ([J)Z
*/
JNIEXPORT jboolean JNICALL Java_cxro_common_device_Pixis_Pixis_shtr_1open_1normal
(JNIEnv *env, jobject obj, jlong handle)
{
    Pixis* cam = reinterpret_cast<Pixis*>(handle);
    return cam->shtr_open_normal();
}

/*
 * Class:     cxro_common_device_Pixis_Pixis
 * Method:    get_shtr_open_dly
 * Signature: (J[I)Z
 */
JNIEXPORT jboolean JNICALL Java_cxro_common_device_Pixis_Pixis_get_1shtr_1open_1dly
(JNIEnv *env, jobject obj, jlong handle, jintArray status)
{
    Pixis* cam = reinterpret_cast<Pixis*>(handle);
    unsigned short delay;
    jboolean result = cam->get_shtr_open_dly(&delay);
    jint delay2 = delay;
    env->SetIntArrayRegion(status,0,1,&delay2);
    return result;
}

/*
* Class:     cxro_common_device_Pixis
* Method:    set_shtr_open_dly
* Signature: ([JI)Z
*/
JNIEXPORT jboolean JNICALL Java_cxro_common_device_Pixis_Pixis_set_1shtr_1open_1dly
(JNIEnv *env, jobject obj, jlong handle, jint shtr_open_dly)
{
    Pixis* cam = reinterpret_cast<Pixis*>(handle);
    jboolean result = cam->set_shtr_open_dly((unsigned short)shtr_open_dly);
    return result;
}

/*
 * Class:     cxro_common_device_Pixis_Pixis
 * Method:    get_shtr_close_dly
 * Signature: (J[I)Z
 */
JNIEXPORT jboolean JNICALL Java_cxro_common_device_Pixis_Pixis_get_1shtr_1close_1dly
(JNIEnv *env, jobject obj, jlong handle, jintArray status)
{
    Pixis* cam = reinterpret_cast<Pixis*>(handle);
    unsigned short delay;
    jboolean result = cam->get_shtr_close_dly(&delay);
    jint delay2 = delay;
    env->SetIntArrayRegion(status,0,1,&delay2);
    return result;
}

/*
* Class:     cxro_common_device_Pixis
* Method:    set_shtr_close_dly
* Signature: ([JI)Z
*/
JNIEXPORT jboolean JNICALL Java_cxro_common_device_Pixis_Pixis_set_1shtr_1close_1dly
(JNIEnv *env, jobject obj, jlong handle, jint shtr_close_dly)
{
    Pixis* cam = reinterpret_cast<Pixis*>(handle);
    jboolean result = cam->set_shtr_close_dly((unsigned short)shtr_close_dly);
    return result;
}

/*
 * Class:     cxro_common_device_Pixis_Pixis
 * Method:    get_clear_cycles
 * Signature: (J[I)Z
 */
JNIEXPORT jboolean JNICALL Java_cxro_common_device_Pixis_Pixis_get_1clear_1cycles
(JNIEnv *env, jobject obj, jlong handle, jintArray cycles)
{
    Pixis* cam = reinterpret_cast<Pixis*>(handle);
    unsigned short clear_cycles;
    jboolean result = cam->get_clear_cycles(&clear_cycles);
    jint clear_cycles2 = clear_cycles;
    env->SetIntArrayRegion(cycles,0,1,&clear_cycles2);
    return result;
}

/*
* Class:     cxro_common_device_Pixis
* Method:    set_clear_cycles
* Signature: ([JI)Z
*/
JNIEXPORT jboolean JNICALL Java_cxro_common_device_Pixis_Pixis_set_1clear_1cycles
(JNIEnv *env, jobject obj, jlong handle, jint clear_cycles)
{
    Pixis* cam = reinterpret_cast<Pixis*>(handle);
    jboolean result = cam->set_clear_cycles((unsigned short)clear_cycles);
    return result;
}

/*
 * Class:     cxro_common_device_Pixis_Pixis
 * Method:    get_clear_strips
 * Signature: (J[I)Z
 */
JNIEXPORT jboolean JNICALL Java_cxro_common_device_Pixis_Pixis_get_1clear_1strips
(JNIEnv *env, jobject obj, jlong handle, jintArray strips)
{
    Pixis* cam = reinterpret_cast<Pixis*>(handle);
    unsigned short clear_strips;
    jboolean result = cam->get_clear_strips(&clear_strips);
    jint clear_strips2 = clear_strips;
    env->SetIntArrayRegion(strips,0,1,&clear_strips2);
    return result;
}

/*
* Class:     cxro_common_device_Pixis
* Method:    set_clear_strips
* Signature: ([JI)Z
*/
JNIEXPORT jboolean JNICALL Java_cxro_common_device_Pixis_Pixis_set_1clear_1strips
(JNIEnv *env, jobject obj, jlong handle, jint strips)
{
    Pixis* cam = reinterpret_cast<Pixis*>(handle);
    jboolean result = cam->set_clear_strips((unsigned short)strips);
    return result;
}

/*
 * Class:     cxro_common_device_Pixis_Pixis
 * Method:    get_tmp
 * Signature: (J[F)Z
 */
JNIEXPORT jboolean JNICALL Java_cxro_common_device_Pixis_Pixis_get_1tmp
(JNIEnv *env, jobject obj, jlong handle, jfloatArray status)
{
    Pixis* cam = reinterpret_cast<Pixis*>(handle);
    float tmp;
    jboolean result = cam->get_tmp(&tmp);
    env->SetFloatArrayRegion(status,0,1,const_cast<const jfloat*>(&tmp));
    return result;
}

/*
 * Class:     cxro_common_device_Pixis_Pixis
 * Method:    get_tmp_setpoint
 * Signature: (J[F)Z
 */
JNIEXPORT jboolean JNICALL Java_cxro_common_device_Pixis_Pixis_get_1tmp_1setpoint
(JNIEnv *env, jobject obj, jlong handle, jfloatArray status)
{
    Pixis* cam = reinterpret_cast<Pixis*>(handle);
    float tmp;
    jboolean result = cam->get_tmp_setpoint(&tmp);
    env->SetFloatArrayRegion(status,0,1,const_cast<const jfloat*>(&tmp));
    return result;
}

/*
* Class:     cxro_common_device_Pixis
* Method:    set_tmp_setpoint
* Signature: ([JF)Z
*/
JNIEXPORT jboolean JNICALL Java_cxro_common_device_Pixis_Pixis_set_1tmp_1setpoint
(JNIEnv *env, jobject obj, jlong handle, jfloat tmp_setpoint)
{
    Pixis* cam = reinterpret_cast<Pixis*>(handle);
    jboolean result = cam->set_tmp_setpoint(tmp_setpoint);
    return result;
}

/*
 * Class:     cxro_common_device_Pixis_Pixis
 * Method:    get_max_gain
 * Signature: (J[S)Z
 */
JNIEXPORT jboolean JNICALL Java_cxro_common_device_Pixis_Pixis_get_1max_1gain
(JNIEnv *env, jobject obj, jlong handle, jshortArray status)
{
    Pixis* cam = reinterpret_cast<Pixis*>(handle);
    short maxGain;
    jboolean result = cam->get_max_gain(&maxGain);
    env->SetShortArrayRegion(status,0,1,const_cast<const jshort*>(&maxGain));
    return result;
}

/*
 * Class:     cxro_common_device_Pixis_Pixis
 * Method:    get_gain
 * Signature: (J[S)Z
 */
JNIEXPORT jboolean JNICALL Java_cxro_common_device_Pixis_Pixis_get_1gain
(JNIEnv *env, jobject obj, jlong handle, jshortArray status)
{
    Pixis* cam = reinterpret_cast<Pixis*>(handle);
    short gain;
    jboolean result = cam->get_gain(&gain);
    env->SetShortArrayRegion(status,0,1,const_cast<const jshort*>(&gain));
    return result;
}

/*
* Class:     cxro_common_device_Pixis
* Method:    set_gain
* Signature: ([JS)Z
*/
JNIEXPORT jboolean JNICALL Java_cxro_common_device_Pixis_Pixis_set_1gain
(JNIEnv *env, jobject obj, jlong handle, jshort gain)
{
    Pixis* cam = reinterpret_cast<Pixis*>(handle);
    jboolean result = cam->set_gain(gain);
    return result;
}

/*
 * Class:     cxro_common_device_Pixis_Pixis
 * Method:    get_gain_mult_enable
 * Signature: (J[S)Z
 */
JNIEXPORT jboolean JNICALL Java_cxro_common_device_Pixis_Pixis_get_1gain_1mult_1enable
(JNIEnv *env, jobject obj, jlong handle, jshortArray status)
{
    Pixis* cam = reinterpret_cast<Pixis*>(handle);
    short enabled;
    jboolean result = cam->get_gain_mult_enable(&enabled);
    env->SetShortArrayRegion(status,0,1,const_cast<const jshort*>(&enabled));
    return result;
}

/*
* Class:     cxro_common_device_Pixis
* Method:    set_gain_mult_enable
* Signature: ([JS)Z
*/
JNIEXPORT jboolean JNICALL Java_cxro_common_device_Pixis_Pixis_set_1gain_1mult_1enable
(JNIEnv *env, jobject obj, jlong handle, jshort enabled)
{
    Pixis* cam = reinterpret_cast<Pixis*>(handle);
    jboolean result = cam->set_gain_mult_enable(enabled);
    return result;
}

/*
 * Class:     cxro_common_device_Pixis_Pixis
 * Method:    get_gain_mult_factor
 * Signature: (J[I)Z
 */
JNIEXPORT jboolean JNICALL Java_cxro_common_device_Pixis_Pixis_get_1gain_1mult_1factor
(JNIEnv *env, jobject obj, jlong handle, jintArray status)
{
    Pixis* cam = reinterpret_cast<Pixis*>(handle);
    unsigned short factor;
    jboolean result = cam->get_gain_mult_factor(&factor);
    jint factor2 = factor;
    env->SetIntArrayRegion(status,0,1,&factor2);
    return result;
}

/*
* Class:     cxro_common_device_Pixis
* Method:    set_gain_mult_factor
* Signature: ([JI)Z
*/
JNIEXPORT jboolean JNICALL Java_cxro_common_device_Pixis_Pixis_set_1gain_1mult_1factor
(JNIEnv *env, jobject obj, jlong handle, jint factor)
{
    Pixis* cam = reinterpret_cast<Pixis*>(handle);
    jboolean result = cam->set_gain_mult_factor((unsigned short)factor);
    return result;
}

/*
 * Class:     cxro_common_device_Pixis_Pixis
 * Method:    get_speed
 * Signature: (J[I)Z
 */
JNIEXPORT jboolean JNICALL Java_cxro_common_device_Pixis_Pixis_get_1speed
(JNIEnv *env, jobject obj, jlong handle, jintArray status)
{
    Pixis* cam = reinterpret_cast<Pixis*>(handle);
    unsigned short speed = 0;
    jboolean result = cam->get_speed(&speed);
    jint speed2 = speed;
    env->SetIntArrayRegion(status,0,1,&speed2);
    return result;
}

/*
 * Class:     cxro_common_device_Pixis_Pixis
 * Method:    get_speed_entries
 * Signature: (J[S)Z
 */
JNIEXPORT jboolean JNICALL Java_cxro_common_device_Pixis_Pixis_get_1speed_1entries
(JNIEnv *env, jobject obj, jlong handle, jshortArray status)
{
    Pixis* cam = reinterpret_cast<Pixis*>(handle);
    short speed;
    jboolean result = cam->get_speed_entries(&speed);
    env->SetShortArrayRegion(status,0,1,const_cast<const jshort*>(&speed));
    return result;
}

/*
 * Class:     cxro_common_device_Pixis_Pixis
 * Method:    get_readout_port_entries
 * Signature: (J[S)Z
 */
JNIEXPORT jboolean JNICALL Java_cxro_common_device_Pixis_Pixis_get_1readout_1port_1entries
(JNIEnv *env, jobject obj, jlong handle, jshortArray status)
{
    Pixis* cam = reinterpret_cast<Pixis*>(handle);
    short readout;
    jboolean result = cam->get_readout_port_entries(&readout);
    env->SetShortArrayRegion(status,0,1,const_cast<const jshort*>(&readout));
    return result;
}

/*
 * Class:     cxro_common_device_Pixis_Pixis
 * Method:    get_bits
 * Signature: (J[S)Z
 */
JNIEXPORT jboolean JNICALL Java_cxro_common_device_Pixis_Pixis_get_1bits
(JNIEnv *env, jobject obj, jlong handle, jshortArray bits)
{
    Pixis* cam = reinterpret_cast<Pixis*>(handle);
    short getbits;
    jboolean result = cam->get_bits(&getbits);
    env->SetShortArrayRegion(bits,0,1,const_cast<const jshort*>(&getbits));
    return result;
}

/*
 * Class:     cxro_common_device_Pixis_Pixis
 * Method:    get_speed_mode
 * Signature: (J[S)Z
 */
JNIEXPORT jboolean JNICALL Java_cxro_common_device_Pixis_Pixis_get_1speed_1mode
(JNIEnv *env, jobject obj, jlong handle, jshortArray status)
{
    Pixis* cam = reinterpret_cast<Pixis*>(handle);
    short speed;
    jboolean result = cam->get_speed_mode(&speed);
    env->SetShortArrayRegion(status,0,1,const_cast<const jshort*>(&speed));
    return result;
}

/*
* Class:     cxro_common_device_Pixis
* Method:    set_speed_mode
* Signature: ([JS)Z
*/
JNIEXPORT jboolean JNICALL Java_cxro_common_device_Pixis_Pixis_set_1speed_1mode
(JNIEnv *env, jobject obj, jlong handle, jshort speed)
{
    Pixis* cam = reinterpret_cast<Pixis*>(handle);
    jboolean result = cam->set_speed_mode(speed);
    return result;
}

/*
 * Class:     cxro_common_device_Pixis_Pixis
 * Method:    get_readout_port
 * Signature: (J[S)Z
 */
JNIEXPORT jboolean JNICALL Java_cxro_common_device_Pixis_Pixis_get_1readout_1port
(JNIEnv *env, jobject obj, jlong handle, jshortArray status)
{
    Pixis* cam = reinterpret_cast<Pixis*>(handle);
    short port;
    jboolean result = cam->get_readout_port(&port);
    env->SetShortArrayRegion(status,0,1,const_cast<const jshort*>(&port));
    return result;
}

/*
* Class:     cxro_common_device_Pixis
* Method:    set_readout_port
* Signature: ([JS)Z
*/
JNIEXPORT jboolean JNICALL Java_cxro_common_device_Pixis_Pixis_set_1readout_1port
(JNIEnv *env, jobject obj, jlong handle, jshort port)
{
    Pixis* cam = reinterpret_cast<Pixis*>(handle);
    jboolean result = cam->set_readout_port(port);
    return result;
}

/*
 * Class:     cxro_common_device_Pixis_Pixis
 * Method:    get_retries
 * Signature: (J[I)Z
 */
JNIEXPORT jboolean JNICALL Java_cxro_common_device_Pixis_Pixis_get_1retries
(JNIEnv *env, jobject obj, jlong handle, jintArray status)
{
    Pixis* cam = reinterpret_cast<Pixis*>(handle);
    unsigned short retries;
    jboolean result = cam->get_retries(&retries);
    jint retries2 = retries;
    env->SetIntArrayRegion(status,0,1,&retries2);
    return result;
}

/*
* Class:     cxro_common_device_Pixis
* Method:    set_retries
* Signature: ([JI)Z
*/
JNIEXPORT jboolean JNICALL Java_cxro_common_device_Pixis_Pixis_set_1retries
(JNIEnv *env, jobject obj, jlong handle, jint retries)
{
    Pixis* cam = reinterpret_cast<Pixis*>(handle);
    jboolean result = cam->set_retries((unsigned short)retries);
    return result;
}

/*
 * Class:     cxro_common_device_Pixis_Pixis
 * Method:    get_timeout
 * Signature: (J[I)Z
 */
JNIEXPORT jboolean JNICALL Java_cxro_common_device_Pixis_Pixis_get_1timeout
(JNIEnv *env, jobject obj, jlong handle, jintArray status)
{
    Pixis* cam = reinterpret_cast<Pixis*>(handle);
    unsigned short timeout;
    jboolean result = cam->get_timeout(&timeout);
    jint timeout2 = timeout;
    env->SetIntArrayRegion(status,0,1,&timeout2);
    return result;
}

/*
* Class:     cxro_common_device_Pixis
* Method:    set_timeout
* Signature: ([JI)Z
*/
JNIEXPORT jboolean JNICALL Java_cxro_common_device_Pixis_Pixis_set_1timeout
(JNIEnv *env, jobject obj, jlong handle, jint m_sec)
{
    Pixis* cam = reinterpret_cast<Pixis*>(handle);
    jboolean result = cam->set_timeout((unsigned short)m_sec);
    return result;
}

/*
 * Class:     cxro_common_device_Pixis_Pixis
 * Method:    get_frame_capable
 * Signature: (J[I)Z
 */
JNIEXPORT jboolean JNICALL Java_cxro_common_device_Pixis_Pixis_get_1frame_1capable
(JNIEnv *env, jobject obj, jlong handle, jintArray retCapable)
{
    Pixis* cam = reinterpret_cast<Pixis*>(handle);
    unsigned short capable;
    jboolean result = cam->get_frame_capable(&capable);
    jint capable2 = capable;
    env->SetIntArrayRegion(retCapable,0,1,&capable2);
    return result;
}
