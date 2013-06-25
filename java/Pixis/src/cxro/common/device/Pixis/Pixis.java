/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package cxro.common.device.Pixis;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.util.concurrent.Callable;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;
import java.util.concurrent.LinkedBlockingDeque;

/**
 *
 * @author wcork
 */
public class Pixis
{

    static
    {
        try
        {
            System.loadLibrary("Pixis_JNI");
        }
        catch (UnsatisfiedLinkError e)
        {
            System.err.println("Native code library failed to load.\n" + e);
            throw e;
        }
    }
    private long[] objHandle =
    {
        0
    };
    private long[] buffSize =
    {
        0
    };
    
    private final ExecutorService exec = Executors.newSingleThreadExecutor();
    private Future f_op = null;
    private volatile ByteBuffer imageBuff;
    private Future<ByteBuffer> f_op_exp = null;
    private long lastImageGrabTime;
    private boolean contStartFlag = false;
    private boolean timeoutFlag = false;
    private LinkedBlockingDeque imageQueue;

    //***************************** NATIVE METHODS *****************************
    private native boolean Pixis_Init(long[] handle, int cameraNumber);

    private native boolean Pixis_Uninit(long[] handle);

    private native boolean get_sensor_size(long handle, char[] size); //Array encoded {unsigned short * ser, unsigned short * par}

    private native boolean exp_setup(long handle, int exposure_time, int x0, int x1, int y0, int y1, int xbin, int ybin, long[] stream_size);

    private native boolean exp_start(long handle, ByteBuffer image);

    private native boolean exp_check_status(long handle, int[] status); //Array encoded {short * status, unsigned long * byte_cnt}

    private native boolean exp_halt(long handle, short cam_state);

    private native boolean cont_start(long handle, int exposure_time, int x0, int x1, int y0, int y1, int xbin, int ybin, long[] stream_size);

    private native boolean cont_check_status(long handle, int[] status); //Array encoded: {short* status,int * byte_cnt,int * buffer_cnt}

    private native boolean cont_halt(long handle, short cam_state);

    private native boolean cont_get_frame(long handle, ByteBuffer image);

    private native String get_error(long handle);

    private native String[] get_paramList(long handle);

    //*********************LEGACY******************************
    private native boolean get_ccs_status(long handle, int[] status);

    private native boolean get_shtr_status(long handle, short[] status);

    private native boolean shtr_open_never(long handle);

    private native boolean shtr_open_normal(long handle);

    private native boolean get_shtr_open_dly(long handle, int[] status);

    private native boolean set_shtr_open_dly(long handle, int shtr_open_dly);

    private native boolean get_shtr_close_dly(long handle, int[] shtr_close_dly);

    private native boolean set_shtr_close_dly(long handle, int shtr_close_dly);

    private native boolean get_clear_cycles(long handle, int[] clear_cycles);

    private native boolean set_clear_cycles(long handle, int clear_cycles);

    private native boolean get_clear_strips(long handle, int[] strips);

    private native boolean set_clear_strips(long handle, int strips);

    private native boolean get_tmp(long handle, float[] tmp);

    private native boolean get_tmp_setpoint(long handle, float[] tmp_setpoint);

    private native boolean set_tmp_setpoint(long handle, float tmp_setpoint);

    private native boolean get_max_gain(long handle, short[] max_gain);

    private native boolean get_gain(long handle, short[] gain);

    private native boolean set_gain(long handle, short gain);

    private native boolean get_gain_mult_enable(long handle, short[] enabled);

    private native boolean set_gain_mult_enable(long handle, short enabled);

    private native boolean get_gain_mult_factor(long handle, int[] factor);

    private native boolean set_gain_mult_factor(long handle, int factor);

    private native boolean get_speed(long handle, int[] speed);

    private native boolean get_speed_entries(long handle, short[] entries);

    private native boolean get_readout_port_entries(long handle, short[] entries);

    private native boolean get_bits(long handle, short[] bits);

    private native boolean get_speed_mode(long handle, short[] speed_mode);

    private native boolean set_speed_mode(long handle, short speed);

    private native boolean get_readout_port(long handle, short[] port);

    private native boolean set_readout_port(long handle, short port);

    private native boolean get_retries(long handle, int[] retries);

    private native boolean set_retries(long handle, int retries);

    private native boolean get_timeout(long handle, int[] timeout);

    private native boolean set_timeout(long handle, int m_sec);

    private native boolean get_frame_capable(long handle, int[] capable);

    //************************ PUBLIC METHODS ************************
    public Pixis()
    {
        imageQueue = new LinkedBlockingDeque(4);
    }
    
    public boolean initCamera(int index)
    {
        if(!Pixis_Init(objHandle,index))
        {
            return false;
        }
        return true;
    }
    
    public boolean uninitCamera()
    {
        return Pixis_Uninit(objHandle);
    }
    
    public short[] getSensorSize()
    {
        char[] sensor = {0,0};
        //Array encoded {unsigned short * ser, unsigned short * par}
        boolean result = get_sensor_size(objHandle[0],sensor);
        //UPCAST UNSIGNED SHORT TO INT MASK
        return new short[]{(short)(sensor[0] & 0xFFFFFFFF),(short)(sensor[1] & 0xFFFFFFFF)};
    }

    public boolean expSetup(long exposure_time, short x0, short x1, short y0, short y1, short xbin, short ybin)
    {
        boolean status = exp_setup(objHandle[0], (int)exposure_time, x0, x1, y0, y1, xbin, ybin, buffSize);
        buffSize[0] = buffSize[0] & 0xFFFFFFFFL;
        imageBuff = ByteBuffer.allocateDirect((int)(buffSize[0]));
        return status;
    }

    public boolean expStart()
    {
        boolean status = exp_start(objHandle[0], imageBuff);
        f_op_exp = exec.submit(new GrabSingleImage());
        return status;
    }

    public boolean expCheckStatus()
    {
        boolean status = f_op_exp.isDone();
        return status;
    }
    
    public byte[] expGetFrame() throws InterruptedException, ExecutionException
    {
        if (f_op_exp.isDone())
        {
            ByteBuffer temp = f_op_exp.get();
            byte[] ret = new byte[temp.capacity()];
            temp.get(ret);
            return ret;
        }
        return null;
    }

    public boolean expHalt(short cam_state)
    {
        boolean status = exp_halt(objHandle[0], cam_state);
        imageBuff.flip();
        f_op_exp = null;
        imageQueue.clear();
        return status;
    }

    public boolean contStart(long exposureTime, short x0, short x1, short y0, short y1, short xbin, short ybin)
    {
        boolean status = cont_start(objHandle[0], (int) exposureTime, x0, x1, y0, y1, xbin, ybin, buffSize);
        if (status)
        {
            contStartFlag = true;
            timeoutFlag = true;
            buffSize[0] = buffSize[0] & 0xFFFFFFFFL;
            imageBuff = ByteBuffer.allocateDirect((int) buffSize[0]);
            f_op = exec.submit(new GrabImagesContiniously());
        }
        else
        {
            contStartFlag = false;
        }
        
        return status;
    }

    public boolean contCheckStatus() throws IOException
    {
        if(!contStartFlag)
        {
            throw new IOException("Continuous capture is not running.");
        }
        if (timeoutFlag && contStartFlag)
        {
            timeoutFlag = false;
            if (f_op != null)
            {
                f_op.cancel(true);
            }
            f_op = exec.submit(new GrabImagesContiniously());
        }
        return !imageQueue.isEmpty();
    }

    public boolean contHalt(short camState)
    {
        contStartFlag = false;
        timeoutFlag = true;
        if (f_op != null)
        {
            f_op.cancel(true);
        }
        imageBuff.flip();
        imageQueue.clear();
        return cont_halt(objHandle[0], camState);
    }

    public byte[] contGetFrame() throws IOException
    {
        byte[] img = null;
        ByteBuffer tmp = null;
        if (timeoutFlag)
        {
            throw new IOException("Capture thread has timed out.");
        }
        lastImageGrabTime = System.currentTimeMillis();
        if (!imageQueue.isEmpty())
        {
            tmp = (ByteBuffer) imageQueue.pollLast();
            img = new byte[imageBuff.capacity()];
            tmp.get(img);
            return img;
        }
        return null;
    }

    public String getError()
    {
        return get_error(objHandle[0]);
    }

    public String[] getParamList()
    {
        return get_paramList(objHandle[0]);
    }

    //*********************LEGACY******************************
    public int getCcsStatus() throws IOException
    {
        int[] status = new int[1];
        if(!get_ccs_status(objHandle[0],status))
        {
            throw new IOException("Communication failure");
        }
        return status[0];
    }

    public short getShtrStatus() throws IOException
    {
        short[] status = new short[1];
        if(!get_shtr_status(objHandle[0],status))
        {
            throw new IOException("Communication failure");
        }
        return status[0];
    }

    public boolean shtrOpenNever()
    {
        return shtr_open_never(objHandle[0]);
    }

    public boolean shtrOpenNormal()
    {
        return shtr_open_normal(objHandle[0]);
    }

    public int getShtrOpenDly() throws IOException
    {
        int[] delay = new int[1];
        if(!get_shtr_open_dly(objHandle[0],delay))
        {
            throw new IOException("Communication failure");
        }
        return delay[0];
    }

    public boolean setShtrOpenDly(int shtrOpenDly)
    {
        return set_shtr_open_dly(objHandle[0],shtrOpenDly);
    }

    public int getShtrCloseDly() throws IOException
    {
        int[] delay = new int[1];
        if(!get_shtr_close_dly(objHandle[0],delay))
        {
            throw new IOException("Communication failure");
        }
        return delay[0];
    }

    public boolean setShtrCloseDly(int shtrCloseDly)
    {
        return set_shtr_close_dly(objHandle[0], shtrCloseDly);
    }

    public int getClearCycles() throws IOException
    {
        int[] cycles = new int[1];
        if(!get_clear_cycles(objHandle[0],cycles))
        {
            throw new IOException("Communication failure");
        }
        return cycles[0];
    }

    public boolean setClearCycles(int clearCycles)
    {
        return set_clear_cycles(objHandle[0],clearCycles);
    }

    public int getClearStrips() throws IOException
    {
        int[] strips = new int[1];
        if(!get_clear_strips(objHandle[0],strips))
        {
            throw new IOException("Communication failure");
        }
        return strips[0];
    }

    public boolean setClearStrips(int strips)
    {
        return set_clear_strips(objHandle[0],strips);
    }

    public float getTmp() throws IOException
    {
        float[] tmp = new float[1];
        if(!get_tmp(objHandle[0],tmp))
        {
            throw new IOException("Communication failure");
        }
        return tmp[0];
    }

    public float getTmpSetpoint() throws IOException
    {
        float[] point = new float[1];
        if(!get_tmp_setpoint(objHandle[0],point))
        {
            throw new IOException("Communication failure");
        }
        return point[0];
    }

    public boolean setTmpSetpoint(float tmpSetpoint)
    {
        return set_tmp_setpoint(objHandle[0], tmpSetpoint);
    }

    public int getMaxGain() throws IOException
    {
        short[] gain = new short[1];
        if(!get_max_gain(objHandle[0],gain))
        {
            throw new IOException("Communication failure");
        }
        return gain[0];
    }

    public int getGain() throws IOException
    {
        short[] gain = new short[1];
        if(!get_gain(objHandle[0],gain))
        {
            throw new IOException("Communication failure");
        }
        return gain[0];
    }

    public boolean setGain(int gain)
    {
        return set_gain(objHandle[0], (short)gain);
    }

    public int getGainMultEnable() throws IOException
    {
        short[] enable = new short[1];
        if(!get_gain_mult_enable(objHandle[0],enable))
        {
            throw new IOException("Communication failure");
        }
        return enable[0];
    }

    public boolean setGainMultEnable(int enabled)
    {
        return set_gain_mult_enable(objHandle[0], (short)enabled);
    }

    public int getGainMultFactor() throws IOException
    {
        int[] factor = new int[1];
        if(!get_gain_mult_factor(objHandle[0],factor))
        {
            throw new IOException("Communication failure");
        }
        return factor[0];
    }

    public boolean setGainMultFactor(int factor)
    {
        return set_gain_mult_factor(objHandle[0], factor);
    }

    public int getSpeed() throws IOException
    {
        int[] speed = new int[1];
        if(!get_speed(objHandle[0],speed))
        {
            throw new IOException("Communication failure");
        }
        return speed[0];
    }

    public int getSpeedEntries() throws IOException
    {
        short[] speed = new short[1];
        if(!get_speed_entries(objHandle[0],speed))
        {
            throw new IOException("Communication failure");
        }
        return speed[0];
    }

    public int getReadoutPortEntries() throws IOException
    {
        short[] entries = new short[1];
        if(!get_readout_port_entries(objHandle[0],entries))
        {
            throw new IOException("Communication failure");
        }
        return entries[0];
    }

    public int getBits() throws IOException
    {
        short[] bits = new short[1];
        if(!get_bits(objHandle[0],bits))
        {
            throw new IOException("Communication failure");
        }
        return bits[0];
    }

    public short getSpeedMode() throws IOException
    {
        short[] speed = new short[1];
        if(!get_speed_mode(objHandle[0],speed))
        {
            throw new IOException("Communication failure");
        }
        return speed[0];
    }

    public boolean setSpeedMode(short speed)
    {
        return set_speed_mode(objHandle[0], speed);
    }

    public short getReadoutPort() throws IOException
    {
        short[] port = new short[1];
        if(!get_readout_port(objHandle[0],port))
        {
            throw new IOException("Communication failure");
        }
        return port[0];
    }

    public boolean setReadoutport(short port)
    {
        return set_readout_port(objHandle[0], port);
    }

    public int getRetries() throws IOException
    {
        int[] retries = new int[1];
        if(!get_retries(objHandle[0],retries))
        {
            throw new IOException("Communication failure");
        }
        return retries[0];
    }

    public boolean setRetries(int retries)
    {
        return set_retries(objHandle[0], retries);
    }

    public int getTimeout() throws IOException
    {
        int[] timeout = new int[1];
        if(!get_timeout(objHandle[0],timeout))
        {
            throw new IOException("Communication failure");
        }
        return timeout[0];
    }

    public boolean setTimeout(int mSec)
    {
        return set_timeout(objHandle[0], mSec);
    }

    public int getFrameCapable() throws IOException
    {
        int[] capable = new int[1];
        if(!get_frame_capable(objHandle[0],capable))
        {
            throw new IOException("Communication failure");
        }
        return capable[0];
    }
    
    /**
     * Grabs images using blocking get and pushes to a fifo circular buffer.
     */
    private class GrabImagesContiniously implements Runnable
    {
        int[] sta;
        boolean comStat;
        
        public GrabImagesContiniously()
        {
            timeoutFlag = false;
            comStat = false;
            sta = new int[]
            {
                0, 0, 0
            };
            lastImageGrabTime = System.currentTimeMillis();
            System.out.println("CONTINIOUS CAP THREAD CREATED.");
        }

        @Override
        public void run()
        {
            try
            {
                //LOOP HERE FOR EVER
                while (true)
                {
                    if ((System.currentTimeMillis() - lastImageGrabTime) >= 10000)
                    {
                        imageQueue.clear();
                        timeoutFlag = true;
                        throw new InterruptedException("Timout Reached.");
                    }
                    //Array encoded: {short* status,int * byte_cnt,int * buffer_cnt}
                    Thread.sleep(50);
                    comStat = cont_check_status(objHandle[0], sta);
                    if (comStat && sta[0] == 3)
                    {
                        cont_get_frame(objHandle[0], imageBuff);
                        if (!imageQueue.isEmpty())
                        {
                            imageQueue.removeLast();
                        }
                        imageQueue.push(imageBuff.duplicate());
                    }
                    else if (!comStat) //Fail on read.
                    {
                        //String err = "Stat: " + sta[0] + " Error: " + get_error(objHandle[0]);
                        contStartFlag = false;
                        timeoutFlag = true;
                        cont_halt(objHandle[0], (short) 2); //Kill read.
                        imageQueue.clear(); //Kill buffer
                        //throw new InterruptedException(err);
                    }
                    Thread.sleep(100);
                }
            }
            catch (InterruptedException ex)
            {
                //System.out.println("CAP THREAD KILLED : " + ex);
                //From Thread.sleep
                //Ignore interrupted sleep
            }
        }
    }

    /**
     * Grabs an image from the camera in the background.
     */
    private class GrabSingleImage implements Callable
    {

        private int[] sta;
        
        public GrabSingleImage()
        {
            sta = new int[]{0,0};
        }

        public ByteBuffer call()
        {
            try
            {
                //Array encoded {short * status, unsigned long * byte_cnt})
                while (!(exp_check_status(objHandle[0], sta)) || sta[0] != 3)
                {
                    Thread.sleep(20);
                }
                if (!imageBuff.hasRemaining())
                {
                    return null;
                }
            }
            catch (InterruptedException ex)
            {
                //From Thread.sleep
                //Ignore interrupted sleep
            }
            return imageBuff;
        }
    }

    protected void finalize()
    {
        //camera.stopCapture();
        this.uninitCamera();
        imageQueue.clear();
    }
}
