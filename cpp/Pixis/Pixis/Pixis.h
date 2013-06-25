// Pixis.h

#pragma once

#include <string>
#include <vector>
#include <exception>

//using namespace System;

class Pixis
{
private:
    short hcam;
    char name[1000];
    unsigned long frame_size;
    unsigned short *circular_buffer;
    bool captureMode; //{0} Single, {1} Continuous
public:

    Pixis(short hcam);
    ~Pixis();

    bool get_sensor_size(unsigned short * ser, unsigned short * par);

    bool exp_setup(unsigned long exposure_time,unsigned short x0,unsigned short x1,unsigned short y0,unsigned short y1,unsigned short xbin,unsigned short ybin,unsigned long * stream_size);
    bool exp_start(unsigned short * image);
    bool exp_check_status(short * status, unsigned long * byte_cnt);
    bool exp_halt(short cam_state);

    bool cont_start(unsigned long exposure_time,unsigned short x0,unsigned short x1,unsigned short y0,unsigned short y1,unsigned short xbin,unsigned short ybin,unsigned long * stream_size);
    bool cont_check_status(short* status,unsigned long * byte_cnt,unsigned long * buffer_cnt);
    bool cont_halt(short cam_state);
    bool cont_get_frame(unsigned short * image);

    bool get_error(std::string &error);
    bool get_paramList(std::vector<std::string> &params, short &numP);

    //In testing
    bool get_param(std::string param, double value);
    bool set_param(std::string param, double value);


    /*************************************************************************
    ***THIS IS A TEMPORARY SOLUTION TO THE GET/SET PARAM SYSTEM.
    ***ONCE COMPLETED, DELETE ALL METHODS FROM HERE DOWN!!!
    **************************************************************************/
    //*********************LEGACY******************************
    bool get_ccs_status(unsigned short *ccs_status);
    bool get_shtr_status(short *shtr_status);
    bool shtr_open_never();
    bool shtr_open_normal();
    bool get_shtr_open_dly(unsigned short *shtr_open_dly);
    bool set_shtr_open_dly(unsigned short shtr_open_dly);
    bool get_shtr_close_dly(unsigned short *shtr_close_dly);
    bool set_shtr_close_dly(unsigned short shtr_close_dly);
    bool get_clear_cycles(unsigned short *cycles);
    bool set_clear_cycles(unsigned short clear_cycles);
    bool get_clear_strips(unsigned short *strips);
    bool set_clear_strips(unsigned short strips);
    bool get_tmp(float *cur_tmp);
    bool get_tmp_setpoint(float *tmp_setpoint);
    bool set_tmp_setpoint(float tmp_setpoint);
    bool get_max_gain(short *max_gain);
    bool get_gain(short *gain);
    bool set_gain(short gain);
    bool get_gain_mult_enable(short *enabled);
    bool set_gain_mult_enable(short enabled);
    bool get_gain_mult_factor(unsigned short *factor);
    bool set_gain_mult_factor(unsigned short factor);
    bool get_speed(unsigned short *speed);
    bool get_speed_entries(short *entries);
    bool get_readout_port_entries(short *entries);
    bool get_bits(short *bits);
    bool get_speed_mode(short *speed);
    bool set_speed_mode(short speed);
    bool get_readout_port(short *port);
    bool set_readout_port(short port);
    bool get_retries(unsigned short *retries);
    bool set_retries(unsigned short retries);
    bool get_timeout(unsigned short *m_sec);
    bool set_timeout(unsigned short m_sec);
    bool get_frame_capable(unsigned short *frame_capable);
};