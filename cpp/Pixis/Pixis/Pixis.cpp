// Pixis.cpp : Simple wrapper for PI ACTON pvcam library.
//
///////////////////////////////////////////////////////////////////////////////
/// @mainpage
/// Wrappers for all pvcam functions (version 2.7)
/// Written June 2013 by William Cork based on original ipvcam library from Bob Gunion.
///
///
/// Copyright (c) 2013 Lawrence Berkeley National Laboratory
///////////////////////////////////////////////////////////////////////////////

#include "master.h"
#include "pvcam.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <map>
#include <stdexcept>
#include "Pixis.h"

using namespace std;

class CamException : public std::runtime_error
{
public:

    CamException(std::string const &error) : std::runtime_error(error)
    {

    }
};

struct EnumInfo
{
    int enumer;
    const char *type;
    bool available;

    union value_t
    {
        int16 i16;
        int32 i32;
        uns16 u16;
        uns32 u32;
        flt64 f64;
        char_ptr cptr;
        rs_bool rbool;
    } value;

    EnumInfo()
    {
    }

    EnumInfo(int enumer, const char *type)
    {
        this->available = false;
        this->enumer = enumer;
        this->type = type;
    }
};

//Contain the strings and their matching enums within an stl map.
std::map<std::string, EnumInfo> enumParser;

char* errBufr = new char[1024];
char* msgBufr = new char[2048];

//--------------- Private Functions -----------------

/**
 * Saves every parameter in the camera;
 * This may only be called after the Pixis constructor is called.
 * 
 * This method and restoreParams are necessary in the case that
 * the pvcam library has kernel panics intermittently unless
 * the camera is reinitialized and loses all parameters.
 * 
 * Currently, this method only stores values that the user is
 * capable of setting in order to reduce the reinitialize time.
 *
 * @param hcam (input) the handle of the camera
 */
void saveParams(short hcam)
{
    //    for(map<string,EnumInfo>::iterator x = enumParser.begin(); x != enumParser.end(); x++)
    //    {
    //        x->second.available = pl_get_param(hcam, x->second.enumer, ATTR_CURRENT, &x->second.value);
    //    }
    enumParser["PARAM_CLEAR_CYCLES"].available = pl_get_param(hcam, PARAM_CLEAR_CYCLES, ATTR_CURRENT, &enumParser["PARAM_CLEAR_CYCLES"].value);
    enumParser["PARAM_NUM_OF_STRIPS_PER_CLR"].available = pl_get_param(hcam, PARAM_NUM_OF_STRIPS_PER_CLR, ATTR_CURRENT, &enumParser["PARAM_NUM_OF_STRIPS_PER_CLR"].value);
    enumParser["PARAM_GAIN_INDEX"].available = pl_get_param(hcam, PARAM_GAIN_INDEX, ATTR_CURRENT, &enumParser["PARAM_GAIN_INDEX"].value);
    enumParser["PARAM_GAIN_MULT_ENABLE"].available = pl_get_param(hcam, PARAM_GAIN_MULT_ENABLE, ATTR_CURRENT, &enumParser["PARAM_GAIN_MULT_ENABLE"].value);
    enumParser["PARAM_GAIN_MULT_FACTOR"].available = pl_get_param(hcam, PARAM_GAIN_MULT_FACTOR, ATTR_CURRENT, &enumParser["PARAM_GAIN_MULT_FACTOR"].value);
    enumParser["PARAM_READOUT_PORT"].available = pl_get_param(hcam, PARAM_READOUT_PORT, ATTR_CURRENT, &enumParser["PARAM_READOUT_PORT"].value);
    enumParser["PARAM_DD_RETRIES"].available = pl_get_param(hcam, PARAM_DD_RETRIES, ATTR_CURRENT, &enumParser["PARAM_DD_RETRIES"].value);
    enumParser["PARAM_SHTR_CLOSE_DELAY"].available = pl_get_param(hcam, PARAM_SHTR_CLOSE_DELAY, ATTR_CURRENT, &enumParser["PARAM_SHTR_CLOSE_DELAY"].value);
    enumParser["PARAM_SHTR_OPEN_DELAY"].available = pl_get_param(hcam, PARAM_SHTR_OPEN_DELAY, ATTR_CURRENT, &enumParser["PARAM_SHTR_OPEN_DELAY"].value);
    enumParser["PARAM_SPDTAB_INDEX"].available = pl_get_param(hcam, PARAM_SPDTAB_INDEX, ATTR_CURRENT, &enumParser["PARAM_SPDTAB_INDEX"].value);
    enumParser["PARAM_DD_TIMEOUT"].available = pl_get_param(hcam, PARAM_DD_TIMEOUT, ATTR_CURRENT, &enumParser["PARAM_DD_TIMEOUT"].value);
    enumParser["PARAM_TEMP_SETPOINT"].available = pl_get_param(hcam, PARAM_TEMP_SETPOINT, ATTR_CURRENT, &enumParser["PARAM_TEMP_SETPOINT"].value);
}

/**
 * Restores every parameter in the camera;
 * This may only be called after the Pixis constructor is called.
 * 
 * This method and saveParams are necessary in the case that
 * the pvcam library has kernel panics intermittently unless
 * the camera is reinitialized and loses all parameters.
 * 
 * Currently, this method only stores values that the user is
 * capable of setting in order to reduce the reinitialize time.
 *
 * @param hcam (input) the handle of the camera
 */
void restoreParams(short hcam)
{
    //    for(map<string,EnumInfo>::iterator x = enumParser.begin(); x != enumParser.end(); x++)
    //    {
    //        if(x->second.available)
    //        {
    //                pl_set_param(hcam, x->second.enumer, &x->second.value);
    //        }
    //    }
    pl_set_param(hcam, PARAM_CLEAR_CYCLES, &enumParser["PARAM_CLEAR_CYCLES"].value);
    pl_set_param(hcam, PARAM_NUM_OF_STRIPS_PER_CLR, &enumParser["PARAM_NUM_OF_STRIPS_PER_CLR"].value);
    pl_set_param(hcam, PARAM_GAIN_INDEX, &enumParser["PARAM_GAIN_INDEX"].value);
    pl_set_param(hcam, PARAM_GAIN_MULT_ENABLE, &enumParser["PARAM_GAIN_MULT_ENABLE"].value);
    pl_set_param(hcam, PARAM_GAIN_MULT_FACTOR, &enumParser["PARAM_GAIN_MULT_FACTOR"].value);
    pl_set_param(hcam, PARAM_READOUT_PORT, &enumParser["PARAM_READOUT_PORT"].value);
    pl_set_param(hcam, PARAM_DD_RETRIES, &enumParser["PARAM_DD_RETRIES"].value);
    pl_set_param(hcam, PARAM_SHTR_CLOSE_DELAY, &enumParser["PARAM_SHTR_CLOSE_DELAY"].value);
    pl_set_param(hcam, PARAM_SHTR_OPEN_DELAY, &enumParser["PARAM_SHTR_OPEN_DELAY"].value);
    pl_set_param(hcam, PARAM_SPDTAB_INDEX, &enumParser["PARAM_SPDTAB_INDEX"].value);
    pl_set_param(hcam, PARAM_DD_TIMEOUT, &enumParser["PARAM_DD_TIMEOUT"].value);
    pl_set_param(hcam, PARAM_TEMP_SETPOINT, &enumParser["PARAM_TEMP_SETPOINT"].value);
}

/**
 * Quickly reinitializes the camera and re-opens the connection.
 * This may only be called after the Pixis constructor is called.
 *
 * @param name (input) the name of the camera
 * @param hcam (input) the handle of the camera
 */
void fastReconnect(char* name, short &hcam)
{
    saveParams(hcam); //Save all changeable params from camera

    pl_cam_close(hcam); // Close the camera's connection.
    pl_exp_uninit_seq(); // Uninitialize data collection.
    pl_pvcam_uninit();
    pl_pvcam_init();
    pl_cam_open(name, &hcam, OPEN_EXCLUSIVE);

    restoreParams(hcam); //Restore all changeable params from camera
}

//--------------- Constructors/Destructors -----------------

Pixis::Pixis(short hcam)
{
    //Setup enumeration maps (Work in Progress)
    {
        //Types:
        //int16
        //int32
        //uns16
        //uns32
        //flt64
        //char_ptr
        //rs_bool
        //enum
        //???
        enumParser["PARAM_DD_INFO_LENGTH"] = EnumInfo(PARAM_DD_INFO_LENGTH, "int16"); //int16
        enumParser["PARAM_DD_VERSION"] = EnumInfo(PARAM_DD_VERSION, "uns16"); //uns16
        enumParser["PARAM_DD_RETRIES"] = EnumInfo(PARAM_DD_RETRIES, "uns16"); //uns16
        enumParser["PARAM_DD_TIMEOUT"] = EnumInfo(PARAM_DD_TIMEOUT, "uns16"); //uns16
        enumParser["PARAM_DD_INFO"] = EnumInfo(PARAM_DD_INFO, "char_ptr"); //char_ptr
        enumParser["PARAM_MIN_BLOCK"] = EnumInfo(PARAM_MIN_BLOCK, "int16"); //
        enumParser["PARAM_NUM_MIN_BLOCK"] = EnumInfo(PARAM_NUM_MIN_BLOCK, "int16");
        enumParser["PARAM_SKIP_AT_ONCE_BLK"] = EnumInfo(PARAM_SKIP_AT_ONCE_BLK, "int32");
        enumParser["PARAM_NUM_OF_STRIPS_PER_CLR"] = EnumInfo(PARAM_NUM_OF_STRIPS_PER_CLR, "int16");
        enumParser["PARAM_CONT_CLEARS"] = EnumInfo(PARAM_CONT_CLEARS, "???");
        enumParser["PARAM_ANTI_BLOOMING"] = EnumInfo(PARAM_ANTI_BLOOMING, "enum");
        enumParser["PARAM_LOGIC_OUTPUT"] = EnumInfo(PARAM_LOGIC_OUTPUT, "enum");
        enumParser["PARAM_LOGIC_OUTPUT_INVERT"] = EnumInfo(PARAM_LOGIC_OUTPUT_INVERT, "rs_bool");
        enumParser["PARAM_EDGE_TRIGGER"] = EnumInfo(PARAM_EDGE_TRIGGER, "enum");
        enumParser["PARAM_INTENSIFIER_GAIN"] = EnumInfo(PARAM_INTENSIFIER_GAIN, "int16");
        enumParser["PARAM_SHTR_GATE_MODE"] = EnumInfo(PARAM_SHTR_GATE_MODE, "enum");
        enumParser["PARAM_ADC_OFFSET"] = EnumInfo(PARAM_ADC_OFFSET, "int16"); //int16
        enumParser["PARAM_CHIP_NAME"] = EnumInfo(PARAM_CHIP_NAME, "char_ptr");
        enumParser["PARAM_COOLING_MODE"] = EnumInfo(PARAM_COOLING_MODE, "enum");
        enumParser["PARAM_HEAD_COOLING_CTRL"] = EnumInfo(PARAM_HEAD_COOLING_CTRL, "enum");
        enumParser["PARAM_COOLING_FAN_CTRL"] = EnumInfo(PARAM_COOLING_FAN_CTRL, "enum");
        enumParser["PARAM_PREAMP_DELAY"] = EnumInfo(PARAM_PREAMP_DELAY, "uns16");
        enumParser["PARAM_COLOR_MODE"] = EnumInfo(PARAM_COLOR_MODE, "enum");
        enumParser["PARAM_MPP_CAPABLE"] = EnumInfo(PARAM_MPP_CAPABLE, "enum");
        enumParser["PARAM_PREAMP_OFF_CONTROL"] = EnumInfo(PARAM_PREAMP_OFF_CONTROL, "uns32");
        enumParser["PARAM_SERIAL_NUM"] = EnumInfo(PARAM_SERIAL_NUM, "uns16");
        enumParser["PARAM_PREMASK"] = EnumInfo(PARAM_PREMASK, "uns16");
        enumParser["PARAM_PRESCAN"] = EnumInfo(PARAM_PRESCAN, "uns16");
        enumParser["PARAM_POSTMASK"] = EnumInfo(PARAM_POSTMASK, "uns16");
        enumParser["PARAM_POSTSCAN"] = EnumInfo(PARAM_POSTSCAN, "uns16");
        enumParser["PARAM_PIX_PAR_DIST"] = EnumInfo(PARAM_PIX_PAR_DIST, "uns16");
        enumParser["PARAM_PIX_PAR_SIZE"] = EnumInfo(PARAM_PIX_PAR_SIZE, "uns16");
        enumParser["PARAM_PIX_SER_DIST"] = EnumInfo(PARAM_PIX_SER_DIST, "uns16");
        enumParser["PARAM_PIX_SER_SIZE"] = EnumInfo(PARAM_PIX_SER_SIZE, "uns16");
        enumParser["PARAM_SUMMING_WELL"] = EnumInfo(PARAM_SUMMING_WELL, "rs_bool");
        enumParser["PARAM_FWELL_CAPACITY"] = EnumInfo(PARAM_FWELL_CAPACITY, "uns32");
        enumParser["PARAM_PAR_SIZE"] = EnumInfo(PARAM_PAR_SIZE, "uns16");
        enumParser["PARAM_SER_SIZE"] = EnumInfo(PARAM_SER_SIZE, "uns16");
        enumParser["PARAM_ACCUM_CAPABLE"] = EnumInfo(PARAM_ACCUM_CAPABLE, "rs_bool"); //rs_bool
        enumParser["PARAM_FTSCAN"] = EnumInfo(PARAM_FTSCAN, "uns16");
        enumParser["PARAM_CUSTOM_CHIP"] = EnumInfo(PARAM_CUSTOM_CHIP, "rs_bool");
        enumParser["PARAM_CUSTOM_TIMING"] = EnumInfo(PARAM_CUSTOM_TIMING, "rs_bool");
        enumParser["PARAM_PAR_SHIFT_TIME"] = EnumInfo(PARAM_PAR_SHIFT_TIME, "int16");
        enumParser["PARAM_SER_SHIFT_TIME"] = EnumInfo(PARAM_SER_SHIFT_TIME, "int16");
        enumParser["PARAM_PAR_SHIFT_INDEX"] = EnumInfo(PARAM_PAR_SHIFT_INDEX, "uns32");
        enumParser["PARAM_KIN_WIN_SIZE"] = EnumInfo(PARAM_KIN_WIN_SIZE, "uns16");
        enumParser["PARAM_CONTROLLER_ALIVE"] = EnumInfo(PARAM_CONTROLLER_ALIVE, "rs_bool");
        enumParser["PARAM_READOUT_TIME"] = EnumInfo(PARAM_READOUT_TIME, "flt64");
        enumParser["PARAM_CLEAR_CYCLES"] = EnumInfo(PARAM_CLEAR_CYCLES, "uns16");
        enumParser["PARAM_CLEAR_MODE"] = EnumInfo(PARAM_CLEAR_MODE, "enum");
        enumParser["PARAM_FRAME_CAPABLE"] = EnumInfo(PARAM_FRAME_CAPABLE, "rs_bool");
        enumParser["PARAM_PMODE"] = EnumInfo(PARAM_PMODE, "rs_bool");
        enumParser["PARAM_CCS_STATUS"] = EnumInfo(PARAM_CCS_STATUS, "int16");
        enumParser["PARAM_TEMP"] = EnumInfo(PARAM_TEMP, "int16");
        enumParser["PARAM_TEMP_SETPOINT"] = EnumInfo(PARAM_TEMP_SETPOINT, "int16");
        enumParser["PARAM_CAM_FW_VERSION"] = EnumInfo(PARAM_CAM_FW_VERSION, "uns16");
        enumParser["PARAM_HEAD_SER_NUM_ALPHA"] = EnumInfo(PARAM_HEAD_SER_NUM_ALPHA, "char_ptr");
        enumParser["PARAM_PCI_FW_VERSION"] = EnumInfo(PARAM_PCI_FW_VERSION, "uns16");
        enumParser["PARAM_CAM_FW_FULL_VERSION"] = EnumInfo(PARAM_CAM_FW_FULL_VERSION, "char_ptr");
        enumParser["PARAM_EXPOSURE_MODE"] = EnumInfo(PARAM_EXPOSURE_MODE, "enum");
        enumParser["PARAM_BIT_DEPTH"] = EnumInfo(PARAM_BIT_DEPTH, "int16");
        enumParser["PARAM_GAIN_INDEX"] = EnumInfo(PARAM_GAIN_INDEX, "int16");
        enumParser["PARAM_SPDTAB_INDEX"] = EnumInfo(PARAM_SPDTAB_INDEX, "int16");
        enumParser["PARAM_READOUT_PORT"] = EnumInfo(PARAM_READOUT_PORT, "enum");
        enumParser["PARAM_PIX_TIME"] = EnumInfo(PARAM_PIX_TIME, "int16");
        enumParser["PARAM_SHTR_CLOSE_DELAY"] = EnumInfo(PARAM_SHTR_CLOSE_DELAY, "uns16");
        enumParser["PARAM_SHTR_OPEN_DELAY"] = EnumInfo(PARAM_SHTR_OPEN_DELAY, "uns16");
        enumParser["PARAM_SHTR_OPEN_MODE"] = EnumInfo(PARAM_SHTR_OPEN_MODE, "enum");
        enumParser["PARAM_SHTR_STATUS"] = EnumInfo(PARAM_SHTR_STATUS, "enum");
        enumParser["PARAM_SHTR_CLOSE_DELAY_UNIT"] = EnumInfo(PARAM_SHTR_CLOSE_DELAY_UNIT, "enum");
        enumParser["PARAM_SHTR_RES"] = EnumInfo(PARAM_SHTR_RES, "enum");
        enumParser["PARAM_IO_ADDR"] = EnumInfo(PARAM_IO_ADDR, "uns16");
        enumParser["PARAM_IO_TYPE"] = EnumInfo(PARAM_IO_TYPE, "enum");
        enumParser["PARAM_IO_DIRECTION"] = EnumInfo(PARAM_IO_DIRECTION, "enum");
        enumParser["PARAM_IO_STATE"] = EnumInfo(PARAM_IO_STATE, "flt64");
        enumParser["PARAM_IO_BITDEPTH"] = EnumInfo(PARAM_IO_BITDEPTH, "uns16");
        enumParser["PARAM_GAIN_MULT_FACTOR"] = EnumInfo(PARAM_GAIN_MULT_FACTOR, "uns16");
        enumParser["PARAM_GAIN_MULT_ENABLE"] = EnumInfo(PARAM_GAIN_MULT_ENABLE, "rs_bool");
        enumParser["PARAM_EXP_TIME"] = EnumInfo(PARAM_EXP_TIME, "uns16");
        enumParser["PARAM_EXP_RES"] = EnumInfo(PARAM_EXP_RES, "uns16");
        enumParser["PARAM_EXP_MIN_TIME"] = EnumInfo(PARAM_EXP_MIN_TIME, "flt64");
        enumParser["PARAM_EXP_RES_INDEX"] = EnumInfo(PARAM_EXP_RES_INDEX, "enum");
        enumParser["PARAM_BOF_EOF_ENABLE"] = EnumInfo(PARAM_BOF_EOF_ENABLE, "enum");
        enumParser["PARAM_BOF_EOF_COUNT"] = EnumInfo(PARAM_BOF_EOF_COUNT, "uns32");
        enumParser["PARAM_BOF_EOF_CLR"] = EnumInfo(PARAM_BOF_EOF_CLR, "rs_bool");
        enumParser["PARAM_CIRC_BUFFER"] = EnumInfo(PARAM_CIRC_BUFFER,"rs_bool");
        enumParser["PARAM_HW_AUTOSTOP"] = EnumInfo(PARAM_HW_AUTOSTOP, "int16");
    }

    int16 total_cams; // The number of cameras connected.
    circular_buffer = NULL;
    frame_size = 0;

    //Set the first capture mode to Single
    captureMode = false;

    // keep id
    this->hcam = hcam;

    // Initialize driver
    if (!pl_pvcam_init())
    {
        pl_error_message(pl_error_code(), errBufr);
        sprintf(msgBufr, "pl_pvcam_init failed: %s", errBufr);
        std::string err;
        this->get_error(err);
        throw new CamException(msgBufr);
    }

    //Re-open connection in to grab exclusive use of camera.
    //This is done in the case that a segment fault occurred
    //in a previous process and the connection remained open.
    pl_cam_close(hcam); // Close the camera's connection.
    pl_exp_uninit_seq(); // Uninitialize data collection.
    //Re-Init the Camera.
    pl_pvcam_uninit();
    pl_pvcam_init();

    /* Check the number of cameras. */
    if (!pl_cam_get_total(&total_cams) || (total_cams <= 0))
    {
        pl_exp_uninit_seq(); /* Uninitialize data collection. */
        pl_pvcam_uninit(); /* Uninitialize the driver. */
        throw new CamException("pl_cam_get_total failed: Camera total <= 0");
    }

    /* Get the camera's name. */
    if (!pl_cam_get_name(hcam, name)) /* pass hcam instead of 0*/
    {
        pl_exp_uninit_seq(); /* Uninitialize data collection. */
        pl_pvcam_uninit(); /* Uninitialize the driver. */
        throw new CamException("pl_cam_get_name failed");
    }

    /* Open the camera's connection. */
    if (!pl_cam_open(name, &hcam, OPEN_EXCLUSIVE))
    {
        pl_exp_uninit_seq(); /* Uninitialize data collection. */
        pl_pvcam_uninit(); /* Uninitialize the driver. */
        pl_error_message(pl_error_code(), errBufr);
        sprintf(msgBufr, "pl_cam_open failed: %s", errBufr);
        throw new CamException(msgBufr);
    }

    /* Check to make sure the camera is on and connected */
    if (!pl_cam_get_diags(hcam))
    {
        printf("The camera is not ready.\n");
        pl_cam_close(hcam); /* Close the camera's connection. */
        pl_exp_uninit_seq(); /* Uninitialize data collection. */
        pl_pvcam_uninit(); /* Uninitialize the driver. */
        throw new CamException("pl_cam_get_diags failed");
    }

    // Success
}

Pixis::~Pixis()
{
    pl_cam_close(hcam); // Close the camera's connection.
    pl_exp_uninit_seq(); // Uninitialize data collection.
    pl_pvcam_uninit(); // Uninitialize the driver.
    if (circular_buffer != NULL)
    {
        delete[] circular_buffer; // Safe delete buffer
        circular_buffer = NULL;
    }
}

//--------------- Configuration Methods -----------------

/**
 * Retrieve the size of the ccd
 * <br/>Referenced pvcam functions:
 *  - pl_ccd_get_ser_size
 *  - pl_ccd_get_par_size
 * @param ser (output) the ccd size in the serial direction, pixels
 * @param par (output) the ccd size in the parallel direction, pixels
 * @return true on success, false on failure
 */
bool Pixis::get_sensor_size(unsigned short * ser, unsigned short * par)
{
    if (!pl_ccd_get_ser_size(hcam, ser))
    {
        return (false);
    }
    if (!pl_ccd_get_par_size(hcam, par))
    {
        return (false);
    }

    return true;
}

//-------------- Single Exposure ------------------------

bool Pixis::exp_setup(
        unsigned long exposure_time,
        unsigned short s1,
        unsigned short s2,
        unsigned short p1,
        unsigned short p2,
        unsigned short sbin,
        unsigned short pbin,
        unsigned long * stream_size)
{
    rgn_type region;
    region.s1 = s1;
    region.s2 = s2;
    region.p1 = p1;
    region.p2 = p2;
    region.sbin = sbin;
    region.pbin = pbin;

    //This re-initialises the camera.
    //This seems necessary in Linux in order to make
    //the PVCAM library avoid segfaults.
    //This only seems needed if the user switches capture modes.
    if (captureMode) //if in continuous image mode.
    {
        captureMode = false; //Set to single image mode.
        fastReconnect(name, hcam);
    }

    /* Re-Initialize the data collection functions. */
    pl_exp_init_seq();
    pl_exp_finish_seq(hcam, circular_buffer, NULL);
    pl_exp_uninit_seq();
    pl_exp_init_seq();

    /* Upload the sequence data. */
    if (!pl_exp_setup_seq(hcam, 1, 1, &region, TIMED_MODE, exposure_time, stream_size))
    {
        return (false);
    }

    return (true);
}

/**
 * Start an exposure
 * <br/>Referenced pvcam function:
 *  - pl_exp_start_seq
 * @param image the image buffer
 * @return 1 on success, 0 on failure
 * @sa snap_setup
 */
bool Pixis::exp_start(unsigned short * image)
{
    /* Take a picture and send the data to the image buffer. */
    if (!pl_exp_start_seq(hcam, image))
    {
        return (false);
    }

    return (true);
}

/**
 * Check on the status of the exposure
 *
 * <br/>Referenced pvcam function:
 *  - pl_exp_check_status
 * @param status (output)  the status of the exposure.  One of:
 *   - (0) READOUT_NOT_ACTIVE: System is idle, no data is expected
 *   - (1) EXPOSURE_IN_PROGRESS: Data collection is active, no data have arrived yet 
 *   - (2) READOUT_IN_PROGRESS: Data collection is active, data have started to arrive
 *   - (3) READOUT_COMPLETE: All expected data have arrived, system has returned to idle
 *   - (4) READOUT_FAILED: Something went wrong.  Function returns 0 and user should
 *     check pl_error_code. 
 *   - (5) ACQUISITION_IN_PROGRESS: For PI cameras only: either EXPOSURE_IN_PROGRESS
 *     or READOUT_IN_PROGRESS (these statuses are not available in PI cameras)
 * @param byte_cnt (output)  number of bytes returned so far
 * @return 1 on success, 0 on failure or READOUT_FAILED
 */
bool Pixis::exp_check_status(short * status, unsigned long * byte_cnt)
{
    return (pl_exp_check_status(hcam, status, byte_cnt) != 0);
}

/**
 * Stop an exposure immediately and (optionally) close the shutter
 *
 * <br/>Referenced pvcam function:
 *  - pl_exp_abort
 * @param cam_state (default CCS_HALT_CLOSE_SHTR)  Force the camera
 * control subsystem (CCS) into one of the states (camera dependent):
 *   - {0) CCS_NO_CHANGE: No change
 *   - (1) CCS_HALT: Halt all CCS activity, put in idle state
 *   - (2) CCS_HALT_CLOSE_SHTR: Close the shutter, then do CCS_HALT
 *   - (3) CCS_CLEAR: Put the CCS in the continuous clearing state
 *   - (4) CCS_CLEAR_CLOSE_SHTR: Close the shutter, then do CCS_CLEAR
 *   - (5) CCS_OPEN_SHTR: Open the shutter, then do CCS_HALT
 *   - (6) CCS_CLEAR_OPEN_SHTR: Open the shutter, then to CCS_CLEAR
 * @return 1 on success, 0 on failure
 */
bool Pixis::exp_halt(short cam_state)
{
    //Finish the sequence in the circular buffer.
    pl_exp_finish_seq(hcam, circular_buffer, NULL); //May be unnecessary in single capture. Ignoring status returned.
    //Unit the collection sequence in the PVCAM library.
    pl_exp_uninit_seq();
    return (pl_exp_abort(hcam, cam_state) != 0);
}

//--------------- Continuous Exposure -----------------------

/**
 * Start a continuous exposure
 *
 * Begins collecting images continuously into a circular buffer.
 * This results in much faster acquisition times than collecting
 * one image at a time.
 * Assumes one region, timed mode
 * <br/>Referenced pvcam functions:
 *  - pl_exp_setup_cont
 *  - pl_exp_start_cont
 * @param exposure_time exposure time in the currently selected resolution
 * @param region the region, consisting of:
 *     - x_min minimum in the x direction
 *     - x_max maximum in the x direction
 *     - y_min minimum in the y direction
 *     - y_max maximum in the y direction
 *     - x_bin (default = 1) binning in the x direction
 *     - y_bin (default = 1) binning in the y direction
 * @param stream_size (output) the required stream size for the buffer
 *        to be passed to snap_cont_get_frame
 * @return 1 on success, 0 on failure
 */
bool Pixis::cont_start(
        unsigned long exposure_time,
        unsigned short s1,
        unsigned short s2,
        unsigned short p1,
        unsigned short p2,
        unsigned short sbin,
        unsigned short pbin,
        unsigned long * stream_size)
{
    rgn_type region;
    region.s1 = s1;
    region.s2 = s2;
    region.p1 = p1;
    region.p2 = p2;
    region.sbin = sbin;
    region.pbin = pbin;

    uns32 buffer_size;

    //This re-initialises the camera.
    //This seems necessary in Linux in order to make
    //the PVCAM library avoid segfaults.
    //This only seems needed if the user switches capture modes.
    if (!captureMode) //if in single image mode.
    {
        captureMode = true; //Set to continuous image mode.
        fastReconnect(name, hcam);
    }


    /* Re-Initialize the data collection functions. */
    pl_exp_init_seq();
    pl_exp_finish_seq(hcam, circular_buffer, NULL);
    pl_exp_uninit_seq();
    pl_exp_init_seq();

    /* Init a sequence set the region, exposure mode and exposure time */
    if (!pl_exp_setup_cont(hcam, 1, &region, TIMED_MODE, exposure_time, stream_size,
            CIRC_OVERWRITE))
    {
        return false;
    }

    /* set up a circular buffer of 10 frames */
    buffer_size = *stream_size * 10;
    if (circular_buffer == NULL)
    {
        circular_buffer = (uns16*) malloc(buffer_size);
        frame_size = *stream_size;
    } else if (frame_size != *stream_size)
    {
        delete[] circular_buffer;
        circular_buffer = (uns16*) malloc(buffer_size);
        frame_size = *stream_size;
    }

    /* Start the acquisition */
    if (!pl_exp_start_cont(hcam, circular_buffer, buffer_size))
    {
        return false;
    }
    return true;
}

/**
 * Check the status of a continuous acquisition.
 *
 * <br/>Referenced pvcam functions:
 *  - pl_exp_check_cont_status
 * @param status (output)  the status of the exposure.  One of:
 *   - (0) READOUT_NOT_ACTIVE: System is idle, no data is expected
 *   - (1) EXPOSURE_IN_PROGRESS: Data collection is active, no data have arrived yet 
 *   - (2) READOUT_IN_PROGRESS: Data collection is active, data have started to arrive
 *   - (3) READOUT_COMPLETE: At least one frame has arrived; safe to call snap_cont_get_frame
 *   - (4) READOUT_FAILED: Something went wrong.  Function returns 0 and user should
 *     check pl_error_code. 
 *   - (5) ACQUISITION_IN_PROGRESS: For PI cameras only: either EXPOSURE_IN_PROGRESS
 *     or READOUT_IN_PROGRESS (these statuses are not available in PI cameras)
 * @param byte_cnt (output)  number of bytes returned so far for the current frame
 * @param buffer_cnt (output) number of frames collected so far
 * @return 1 on success, 0 on failure
 */
bool Pixis::cont_check_status(
        short* status,
        unsigned long * byte_cnt,
        unsigned long * buffer_cnt)
{
    return (pl_exp_check_cont_status(hcam, status, byte_cnt, buffer_cnt) != 0);
}

/**
 * Halt a continuous acquisition cycle
 *
 * Stop continuously collecting images
 * Reference pvcam functions:
 *   - pl_exp_stop_cont
 * @param hcam the camera number
 * @param cam_state (default CCS_HALT_CLOSE_SHTR)  Force the camera
 * control subsystem (CCS) into one of the states (camera dependent):
 *   - {0) CCS_NO_CHANGE: No change
 *   - (1) CCS_HALT: Halt all CCS activity, put in idle state
 *   - (2) CCS_HALT_CLOSE_SHTR: Close the shutter, then do CCS_HALT
 *   - (3) CCS_CLEAR: Put the CCS in the continuous clearing state
 *   - (4) CCS_CLEAR_CLOSE_SHTR: Close the shutter, then do CCS_CLEAR
 *   - (5) CCS_OPEN_SHTR: Open the shutter, then do CCS_HALT
 *   - (6) CCS_CLEAR_OPEN_SHTR: Open the shutter, then to CCS_CLEAR
 * @return 1 on success, 0 on failure
 */
bool Pixis::cont_halt(short cam_state)
{
    //Finish the sequence in the circular buffer.
    pl_exp_finish_seq(hcam, circular_buffer, NULL);
    //Unit the collection sequence in the PVCAM library.
    pl_exp_uninit_seq();
    return (pl_exp_stop_cont(hcam, cam_state) != 0);
}

/**
 * Retrieve the latest frame during a continuous data collection cycle
 *
 * <br/>Referenced pvcam functions:
 *   - pl_exp_get_latest_frame
 * @param image (output) the buffer in which to put the frame.  Must be large
 * enough to hold the frame (see the stream_size parameter for snap_cont_start)
 * @return 1 on success, 0 on failure
 */
bool Pixis::cont_get_frame(unsigned short * image)
{
    uns16* p = NULL;
    if (!pl_exp_get_latest_frame(hcam, (void **) &p))
    {
        return false;
    }
    memcpy(image, p, frame_size);
    return true;
}

/**
 * Retrieve the latest error given by a failed method
 *
 * <br/>Referenced pvcam functions:
 *   - pl_error_code
 * @param error (output) the string in which the user desires the error text.
 * @return 1 on success, 0 on failure
 */
bool Pixis::get_error(std::string &error)
{
    char* msg = new char[1000];
    int16 code = pl_error_code();
    if (!pl_error_message(code, msg))
    {
        return false;
    }
    error = std::string(msg);
    return true;
}

bool Pixis::get_paramList(std::vector<std::string> &params, short &numP)
{
    try
    {
        for (map<string, EnumInfo>::iterator x = enumParser.begin(); x != enumParser.end(); x++)
        {
            params.push_back(x->first);
        }
        numP = params.size();
        return true;
    }    catch (...)
    {
        return false;
    }
}

/**
 * Retrieve the value of any parameter.
 *
 *
 *
 *
 *
 *
 */
bool Pixis::get_param(std::string param, double value)
{
    return false;
}

/**
 * Set the value of any parameter.
 *
 *
 *
 *
 *
 *
 */
bool Pixis::set_param(std::string param, double value)
{
    return false;
}

/*************************************************************************
 ***THIS IS A TEMPORARY SOLUTION TO THE GET/SET PARAM SYSTEM.
 ***ONCE COMPLETED, DELETE ALL METHODS FROM HERE DOWN!!!
 **************************************************************************/

/**
 * Retrieve the status of the camera control subsystem (CCS)
 *
 * <br/>Referenced pvcam function:
 *  - pl_ccs_get_adc_offset (OBSOLETE; replaced with pl_get_param)
 * @param hcam the number of the camera
 * @param ccs_status (output) The CCS status.  One of:
 *   - (0) IDLE
 *   - (1) INITIALIZING
 *   - (2) RUNNING
 *   - (3) CONTINUOUSLY_CLEARING
 * @return 1 on success, 0 on failure
 */
bool Pixis::get_ccs_status(unsigned short *ccs_status)
{
    //if (!pl_ccs_get_status(*hcam, shtr_status))
    if (!pl_get_param(hcam, PARAM_CCS_STATUS, ATTR_CURRENT, ccs_status))
    {
        return (false);
    }

    return (true);
}

/**
 * Retrieve the status of the shutter
 *
 * <br/>Referenced pvcam function:
 *  - pl_shtr_get_status (OBSOLETE; replaced with pl_get_param)
 * @param hcam the number of the camera
 * @param shtr_status (output) The shutter status.  One of:
 *   - (0) SHTR_FAULT: Shutter has overheated
 *   - (1) SHTR_OPENING: Shutter is opening
 *   - (2) SHTR_OPEN: Shutter is open
 *   - (3) SHTR_CLOSING: Shutter is closing
 *   - (4) SHTR_CLOSED: Shutter is closed
 *   - (5) SHTR_UNKNOWN: The system cannot determine the shutter state
 * @return 1 on success, 0 on failure
 */
bool Pixis::get_shtr_status(short *shtr_status)
{
    //if (!pl_shtr_get_status(*hcam, shtr_status))
    if (!pl_get_param(hcam, PARAM_SHTR_STATUS, ATTR_CURRENT, shtr_status))
    {
        return (false);
    }
    return (true);
}

/**
 * Close the shutter, and do not open it during exposures.
 *
 * Equivalent to setting the PARAM_SHTR_OPEN_MODE parameter to
 * OPEN_NEVER
 *
 * <br/>Referenced pvcam function:
 *  - pl_shtr_set_open_mode (OBSOLETE: should be replaced with pl_set_param)
 * @param hcam the camera number
 * @return 1 on success, 0 on failure
 */
bool Pixis::shtr_open_never()
{
    /* Prevents the shutter from opening on exposure. */
    if (!pl_shtr_set_open_mode(hcam, OPEN_NEVER))
    {
        return (false);
    }

    return (true);
}

/**
 * Open the shutter during exposures.
 *
 * Equivalent to setting the PARAM_SHTR_OPEN_MODE parameter to
 * OPEN_PRE_EXPOSURE
 * <br/>Referenced pvcam function:
 *  - pl_shtr_set_open_mode (OBSOLETE: should be replaced with pl_set_param)
 * @param hcam the camera number
 * @return 1 on success, 0 on failure
 */
bool Pixis::shtr_open_normal()
{
    /* Prevents the shutter from opening on exposure. */
    if (!pl_shtr_set_open_mode(hcam, OPEN_PRE_EXPOSURE))
    {
        return (false);
    }
    return (true);
}

/**
 * Retrieve the shutter open delay in milliseconds
 *
 * Equivalent to getting the PARAM_SHTR_OPEN_DELAY parameter
 * <br/>Referenced pvcam function:
 *  - pl_shtr_get_open_dly (OBSOLETE: should be replaced with pl_get_param)
 * @param hcam the camera number
 * @param shtr_open_dly (output) the delay in msec
 * @return 1 on success, 0 on failure
 */
bool Pixis::get_shtr_open_dly(unsigned short *shtr_open_dly)
{
    if (!pl_shtr_get_open_dly(hcam, shtr_open_dly))
    {
        return (false);
    }
    return (true);
}

/**
 * Set the shutter open delay in milliseconds
 *
 * Equivalent to setting the PARAM_SHTR_OPEN_DELAY parameter
 * <br/>Referenced pvcam function:
 *  - pl_shtr_set_open_dly (OBSOLETE: should be replaced with pl_set_param)
 * @param hcam the camera number
 * @param shtr_open_dly the delay in msec
 * @return 1 on success, 0 on failure
 */
bool Pixis::set_shtr_open_dly(unsigned short shtr_open_dly)
{

    if (!pl_shtr_set_open_dly(hcam, shtr_open_dly))
    {
        return (false);
    }
    return (true);
}

/**
 * Retrieve the shutter close delay in milliseconds
 *
 * Equivalent to getting the PARAM_SHTR_CLOSE_DELAY parameter
 * <br/>Referenced pvcam function:
 *  - pl_shtr_get_close_dly (OBSOLETE: should be replaced with pl_get_param)
 * @param hcam the camera number
 * @param shtr_close_dly (output) the delay in msec
 * @return 1 on success, 0 on failure
 */
bool Pixis::get_shtr_close_dly(unsigned short *shtr_close_dly)
{
    if (!pl_shtr_get_close_dly(hcam, shtr_close_dly))
    {
        return (false);
    }
    return (true);
}

/**
 * Set the shutter close delay in milliseconds
 *
 * Equivalent to setting the PARAM_SHTR_CLOSE_DELAY parameter
 * <br/>Referenced pvcam function:
 *  - pl_shtr_set_close_dly (OBSOLETE: should be replaced with pl_set_param)
 * @param hcam the camera number
 * @param shtr_close_dly the delay in msec
 * @return 1 on success, 0 on failure
 */
bool Pixis::set_shtr_close_dly(unsigned short shtr_close_dly)
{
    if (!pl_shtr_set_close_dly(hcam, shtr_close_dly))
    {
        return (false);
    }
    return (true);
}

/**
 * Retrieve the clear cycles parameter
 *
 * Equivalent to getting the PARAM_CLEAR_CYCLES parameter
 * <br/>Referenced pvcam function:
 *  - pl_ccd_get_clear_cycles (OBSOLETE: should be replaced with pl_get_param)
 * @param hcam the camera number
 * @param cycles (output) the number of times the CCD is cleared
 * @return 1 on success, 0 on failure
 */
bool Pixis::get_clear_cycles(unsigned short *cycles)
{
    if (!pl_ccd_get_clear_cycles(hcam, cycles))
    {
        return (false);
    }
    return (true);
}

/**
 * Set the clear cycles parameter
 *
 * Equivalent to setting the PARAM_CLEAR_CYCLES parameter
 * <br/>Referenced pvcam function:
 *  - pl_ccd_set_clear_cycles (OBSOLETE: should be replaced with pl_set_param)
 * @param hcam the camera number
 * @param cycles the number of times the CCD is cleared
 * @return 1 on success, 0 on failure
 */
bool Pixis::set_clear_cycles(unsigned short clear_cycles)
{
    if (!pl_ccd_set_clear_cycles(hcam, clear_cycles))
    {
        return (false);
    }
    return (true);
}

/**
 * Retrieve the clear strips parameter
 *
 * Equivalent to getting the PARAM_NUM_OF_STRIPS_PER_CLR parameter
 * <br/>Referenced pvcam function:
 *  - pl_get_param
 * @param hcam the camera number
 * @param strips (output) the number of strips per clear
 * @return 1 on success, 0 on failure
 */
bool Pixis::get_clear_strips(unsigned short *strips)
{
    if (!pl_get_param(hcam, PARAM_NUM_OF_STRIPS_PER_CLR, ATTR_CURRENT, strips))
    {
        return (false);
    }
    return (true);
}

/**
 * Set the clear strips parameter
 *
 * Equivalent to setting the PARAM_NUM_OF_STRIPS_PER_CLR parameter
 * <br/>Referenced pvcam function:
 *  - pl_set_param
 * @param hcam the camera number
 * @param strips the number of strips per clear
 * @return 1 on success, 0 on failure
 */
bool Pixis::set_clear_strips(unsigned short strips)
{
    if (!pl_set_param(hcam, PARAM_NUM_OF_STRIPS_PER_CLR, &strips))
    {
        return (false);
    }
    return (true);
}

/**
 * Retrieve the current temperature of the ccd
 *
 * Equivalent to getting the PARAM_TEMP parameter (except that this
 * divides by 100 to get degrees centigrade)
 * <br/>Referenced pvcam function:
 *  - pl_ccd_get_tmp (OBSOLETE: should be replaced with pl_get_param)
 * @param hcam the camera number
 * @param cur_tmp (output) the measured temperature of the CCD, in degrees centigrade
 * @return 1 on success, 0 on failure
 */
bool Pixis::get_tmp(float *cur_tmp)
{
    /* Local Variables */
    int16 tmp;
    if (!pl_ccd_get_tmp(hcam, &tmp))
    {
        return (false);
    }
    *cur_tmp = (float) tmp / 100.0f;
    return (true);
}

/**
 * Get the temperature setpoint of the ccd
 *
 * Equivalent to getting the PARAM_TEMP_SETPOINT parameter (except that this
 * divides by 100 to get degrees centigrade)
 * <br/>Referenced pvcam function:
 *  - pl_ccd_get_tmp_setpoint (OBSOLETE: should be replaced with pl_get_param)
 * @param hcam the camera number
 * @param tmp_setpoint (output) the desired temperature of the CCD, in degrees centigrade
 * @return 1 on success, 0 on failure
 */
bool Pixis::get_tmp_setpoint(float *tmp_setpoint)
{
    /* Local Variables */
    int16 tmp_set;
    if (!pl_ccd_get_tmp_setpoint(hcam, &tmp_set))
    {
        return (false);
    }
    *tmp_setpoint = (float) tmp_set / 100.0f;
    return (true);
}

/**
 * Set the temperature setpoint of the ccd
 *
 * Equivalent to setting the PARAM_TEMP_SETPOINT parameter (except that this
 * divides the input by 100 to get degrees centigrade)
 * <br/>Referenced pvcam function:
 *  - pl_ccd_set_tmp_setpoint (OBSOLETE: should be replaced with pl_set_param)
 * @param hcam the camera number
 * @param tmp_setpoint the desired temperature of the CCD, in degrees centigrade
 * @return 1 on success, 0 on failure
 */
bool Pixis::set_tmp_setpoint(float tmp_setpoint)
{
    /* Local Variables */
    int16 tmp_set;
    tmp_set = (int16) (tmp_setpoint * 100.0f);
    if (!pl_ccd_set_tmp_setpoint(hcam, tmp_set))
    {
        return (false);
    }
    return (true);
}

/**
 * Get the maximum gain setting for the camera
 *
 * Equivalent to getting the PARAM_GAIN_INDEX parameter with ATTR_MAX
 * <br/>Referenced pvcam function:
 *  - pl_spdtab_get_max_gain (OBSOLETE: should be replaced with pl_get_param)
 * @param hcam the camera number
 * @param max_gain (output) the maximum gain setting (camera dependent; may
 *  be as high as 16)
 * @return 1 on success, 0 on failure
 */
bool Pixis::get_max_gain(short *max_gain)
{
    if (!pl_spdtab_get_max_gain(hcam, max_gain))
    {
        return (false);
    };
    return (true);
}

/**
 * Get the current gain setting
 *
 * Equivalent to getting the PARAM_GAIN_INDEX parameter
 * <br/>Referenced pvcam function:
 *  - pl_spdtab_get_gain (OBSOLETE: should be replaced with pl_get_param)
 * @param hcam the camera number
 * @param gain (output) the gain setting (the meaning of the gain
 *  is camera dependent)
 * @return 1 on success, 0 on failure
 */
bool Pixis::get_gain(short *gain)
{
    if (!pl_spdtab_get_gain(hcam, gain))
    {
        return (false);
    }
    return (true);
}

/**
 * Set the current gain setting
 *
 * Equivalent to setting the PARAM_GAIN_INDEX parameter
 * <br/>Referenced pvcam function:
 *  - pl_spdtab_set_gain (OBSOLETE: should be replaced with pl_set_param)
 * @param hcam the camera number
 * @param gain the gain setting (the meaning of the gain
 *  is camera dependent)
 * @return 1 on success, 0 on failure
 */
bool Pixis::set_gain(short gain)
{
    if (!pl_spdtab_set_gain(hcam, gain))
    {
        return (false);
    }
    return (true);
}

/**
 * Retrieve the on/off indicator for the gain multiplication functionality
 *
 * Referenced pv_(s|g)et_param ID:
 *  - PARAM_GAIN_MULT_ENABLE
 * @param hcam the number of the camera
 * @param enabled 1 if enabled, 0 if not
 * @return 1 on success, 0 on failure
 */
bool Pixis::get_gain_mult_enable(short *enabled)
{
    if (!pl_get_param(hcam, PARAM_GAIN_MULT_ENABLE, ATTR_CURRENT, enabled))
    {
        return (false);
    }
    return (true);
}

/**
 * Turn the gain multiplication functionality on or off
 *
 * Referenced pv_(s|g)et_param ID:
 *  - PARAM_GAIN_MULT_ENABLE
 * @param hcam the number of the camera
 * @param enabled 1 if enabled, 0 if not
 * @return 1 on success, 0 on failure
 */
bool Pixis::set_gain_mult_enable(short enabled)
{
    if (!pl_set_param(hcam, PARAM_GAIN_MULT_ENABLE, &enabled))
    {
        return (false);
    }
    return (true);
}

/**
 * Retrieve the gain multiplication factor
 *
 * Referenced pv_(s|g)et_param ID:
 *  - PARAM_GAIN_MULT_FACTOR
 * @param hcam the number of the camera
 * @param factor the multiplication factor
 * @return 1 on success, 0 on failure
 */
bool Pixis::get_gain_mult_factor(unsigned short *factor)
{
    if (!pl_get_param(hcam, PARAM_GAIN_MULT_FACTOR, ATTR_CURRENT, factor))
    {
        return (false);
    }
    return (true);
}

/**
 * Set the gain multiplication factor
 *
 * Referenced pv_(s|g)et_param ID:
 *  - PARAM_GAIN_MULT_FACTOR
 * @param hcam the number of the camera
 * @param enabled 1 if enabled, 0 if not
 * @return 1 on success, 0 on failure
 */
bool Pixis::set_gain_mult_factor(unsigned short factor)
{
    if (!pl_set_param(hcam, PARAM_GAIN_MULT_FACTOR, &factor))
    {
        return (false);
    }
    return (true);
}

/**
 * Get the time for each pixel in nanoseconds
 *
 * Equivalent to getting the PARAM_PIX_TIME parameter
 * <br/>Referenced pvcam function:
 *  - pl_spdtab_get_time (OBSOLETE: should be replaced with pl_get_param)
 * @param hcam the camera number
 * @param speed (output) the actual speed for the current speed setting
 * @return 1 on success, 0 on failure
 */
bool Pixis::get_speed(unsigned short *speed)
{
    if (!pl_spdtab_get_time(hcam, speed))
    {
        return (false);
    }
    return (true);
}

/**
 * Get the number of speed table entries
 *
 * Equivalent to getting the PARAM_SPDTAB_INDEX parameter with ATTR_MAX
 * <br/>Referenced pvcam function:
 *  - pl_spdtab_get_entries (OBSOLETE: should be replaced with pl_get_param)
 * @param hcam the camera number
 * @param entries (output) the number of entries in the speed table
 * @return 1 on success, 0 on failure
 */
bool Pixis::get_speed_entries(short *entries)
{
    if (!pl_spdtab_get_entries(hcam, entries))
    {
        return (false);
    }
    return (true);
}

/**
 * Get the number of available readout ports
 *
 * Equivalent to getting the PARAM_READOUT_PORT parameter with ATTR_MAX
 * <br/>Referenced pvcam function:
 *  - pl_get_param
 * @param hcam the camera number
 * @param entries (output) the number of entries in the speed table
 * @return 1 on success, 0 on failure
 */
bool Pixis::get_readout_port_entries(short *entries)
{
    if (!pl_get_param(hcam, PARAM_READOUT_PORT, ATTR_MAX, entries))
    {
        return (false);
    }
    return (true);
}

/**
 * Get the bit depth for the currently selected speed choice.
 *
 * Although this number might range between 6 and
 * 16, the data will always be returned in an unsigned 16-bit
 * word. This value indicates the number of valid bits within
 * that word.
 * Equivalent to getting the PARAM_BIT_DEPTH parameter
 * <br/>Referenced pvcam function:
 *  - pl_spdtab_get_bits (OBSOLETE: should be replaced with pl_get_param)
 * @param hcam the camera number
 * @param bits (output) the bit depth
 * @return 1 on success, 0 on failure
 */
bool Pixis::get_bits(short *bits)
{
    if (!pl_spdtab_get_bits(hcam, bits))
    {
        return (false);
    }
    return (true);
}

/**
 * Get the CCD readout speed from a table of available choices
 *
 * Equivalent to getting the PARAM_SPDTAB_INDEX parameter
 * <br/>Referenced pvcam function:
 *  - pl_spdtab_get_num (OBSOLETE: should be replaced with pl_get_param)
 * @param hcam the camera number
 * @param speed (output) the table index
 * @return 1 on success, 0 on failure
 */
bool Pixis::get_speed_mode(short *speed)
{
    if (!pl_spdtab_get_num(hcam, speed))
    {
        return (false);
    }
    return (true);
}

/**
 * Set the CCD readout speed from a table of available choices.
 *
 * NOTE: The gain will always be reset to 1x after setting this parameter.
 * Equivalent to setting the PARAM_SPDTAB_INDEX parameter
 * <br/>Referenced pvcam function:
 *  - pl_spdtab_set_num (OBSOLETE: should be replaced with pl_set_param)
 * @param hcam the camera number
 * @param speed the table index
 * @return 1 on success, 0 on failure
 */
bool Pixis::set_speed_mode(short speed)
{
    if (!pl_spdtab_set_num(hcam, speed))
    {
        return (false);
    }
    return (true);
}

/**
 * Get the CCD readout port
 *
 * Equivalent to getting the PARAM_READOUT_PORT parameter
 * <br/>Referenced pvcam function:
 *  - pl_get_param
 * Possible readout port values (the available ports are camera-specific;
 * use ccd_get_readout_port_entries to determine the number of available ports):
 * <ul><li>READOUT_PORT_MULT_GAIN = 0</li>
 * <li>READOUT_PORT_NORMAL = 1</li>
 * <li>READOUT_PORT_LOW_NOISE = 2</li>
 * <li>READOUT_PORT_HIGH_CAP = 3</li></ul>
 * @param hcam the camera number
 * @param port (output) the port number
 * @return 1 on success, 0 on failure
 */
bool Pixis::get_readout_port(short *port)
{
    if (!pl_get_param(hcam, PARAM_READOUT_PORT, ATTR_CURRENT, port))
    {
        return (false);
    }
    return (true);
}

/**
 * Set the CCD readout port choice
 *
 * NOTE: The gain, speed, and bit depth may be altered as a result of
 * setting this parameter.  You should maintain a list of available readout
 * ports and associated settings, and set all of them after calling this function.
 * Equivalent to setting the PARAM_READOUT_PORT parameter
 * <br/>Referenced pvcam function:
 *  - pl_set_param
 * @param hcam the camera number
 * @param port the readout port index
 * @return 1 on success, 0 on failure
 */
bool Pixis::set_readout_port(short port)
{
    if (!pl_set_param(hcam, PARAM_READOUT_PORT, &port))
    {
        return (false);
    }
    return (true);
}

/**
 * Get the maximum number of command retries.  This should normally
 * not be changed since it "is matched to the
 * communications link, hardware platform, and operating
 * system" [PVCAM 2.6 Manual, p. 36]
 *
 * Equivalent to getting the PARAM_DD_RETRIES parameter
 * <br/>Referenced pvcam function:
 *  - pl_dd_get_retries (OBSOLETE: should be replaced with pl_get_param)
 * @param hcam the camera number
 * @param retries (output) the maximum number of retries
 * @return 1 on success, 0 on failure
 */
bool Pixis::get_retries(unsigned short *retries)
{
    if (!pl_dd_get_retries(hcam, retries))
    {
        return (false);
    }
    return (true);
}

/**
 * Set the maximum number of command retries.  This should normally
 * not be changed since it "is matched to the
 * communications link, hardware platform, and operating
 * system" [PVCAM 2.6 Manual, p. 36]
 *
 * Equivalent to setting the PARAM_DD_RETRIES parameter
 * <br/>Referenced pvcam function:
 *  - pl_dd_set_retries (OBSOLETE: should be replaced with pl_set_param)
 * @param hcam the camera number
 * @param retries the maximum number of retries
 * @return 1 on success, 0 on failure
 */
bool Pixis::set_retries(unsigned short retries)
{
    if (!pl_dd_set_retries(hcam, retries))
    {
        return (false);
    }
    return (true);
}

/**
 * Get the maximum time to wait for acknowledgement.  This should normally
 * not be changed since it "is matched to the
 * communications link, hardware platform, and operating
 * system" [PVCAM 2.6 Manual, p. 36]
 *
 * Equivalent to getting the PARAM_DD_TIMEOUT parameter
 * <br/>Referenced pvcam function:
 *  - pl_dd_get_timeout (OBSOLETE: should be replaced with pl_get_param)
 * @param hcam the camera number
 * @param m_sec (output) the timeout setting in milliseconds
 * @return 1 on success, 0 on failure
 */
bool Pixis::get_timeout(unsigned short *m_sec)
{
    if (!pl_dd_get_timeout(hcam, m_sec))
    {
        return (false);
    }
    return (true);
}

/**
 * Set the maximum time to wait for acknowledgement.  This should normally
 * not be changed since it "is matched to the
 * communications link, hardware platform, and operating
 * system" [PVCAM 2.6 Manual, p. 36]
 *
 * Equivalent to setting the PARAM_DD_TIMEOUT parameter
 * <br/>Referenced pvcam function:
 *  - pl_dd_set_timeout (OBSOLETE: should be replaced with pl_set_param)
 * @param hcam the camera number
 * @param m_sec the timeout setting in milliseconds
 * @return 1 on success, 0 on failure
 */
bool Pixis::set_timeout(unsigned short m_sec)
{
    if (!pl_dd_set_timeout(hcam, m_sec))
    {
        return (false);
    }
    return (true);
}

/**
 * Determine whether this camera can run in frame transfer mode
 * (set through PARAM_PMODE)
 * 
 * Equivalent to getting the PARAM_FRAME_CAPABLE parameter
 * <br/>Referenced pvcam function:
 *  - pl_ccd_get_frame_capable (OBSOLETE: should be replaced with pl_get_param)
 * @param hcam the camera number
 * @param frame_capable (output) 1 if the camera is frame capable, 0 if not
 * @return 1 on success, 0 on failure
 */
bool Pixis::get_frame_capable(unsigned short *frame_capable)
{
    if (!pl_ccd_get_frame_capable(hcam, frame_capable))
    {
        return (false);
    }
    return (true);
}