/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package cxro.common.device;

import Ice.*;
import cxro.common.device.Pixis.*;
import java.io.IOException;
import java.util.concurrent.*;
import java.util.logging.Level;

/**
 *
 * @author wcork
 */
public class PixisClient
{

    private Ice.Communicator ic;
    private PixisICEPrx camera;
    private ImageContainerHolder image = new ImageContainerHolder();
    private final ExecutorService exec = Executors.newSingleThreadExecutor();
    private Future f_op = null;
    private Future<byte[]> f_op_exp = null;
    private long lastImageGrabTime;
    private boolean contStartFlag = false;
    private boolean timeoutFlag = false;
    private LinkedBlockingDeque imageQueue;

    public PixisClient() throws IOException
    {
        int status = 0;
        imageQueue = new LinkedBlockingDeque(4);

        try
        {
            String[] args = {"--Ice.Config=" + System.getProperty("user.dir") + "/config.proxy"};
            Ice.Properties properties = Ice.Util.createProperties(args);
            Ice.InitializationData id = new Ice.InitializationData();
            id.properties = properties;
            ic = Ice.Util.initialize(id);
            Ice.ObjectPrx base = ic.stringToProxy("Pixis@PixisAdapter");
            //Ice.ObjectPrx base = ic.stringToProxy("Pixis:tcp -h 192.168.100.8 -p 10000");
            camera = PixisICEPrxHelper.checkedCast(base);
            if (camera == null)
            {
                throw new Error("Invalid proxy");
            }
        }
        catch (Ice.LocalException e)
        {
            status = 1;
            throw new IOException(e);
        }
    }

    public boolean initCamera(int index) throws CamException
    {
        boolean status = camera.initCamera(index);
        return status;
    }

    public boolean uninitCamera()
    {
        boolean status = camera.uninitCamera();
        return status;
    }

    public short[] getSensorSize() throws IOException
    {
        ShortHolder xSize = new ShortHolder();
        ShortHolder ySize = new ShortHolder();
        if (!camera.getSensorSize(xSize, ySize))
        {
            throw new IOException("Camera communication failed. Error: " + this.getError());
        }
        short[] size = new short[]
        {
            xSize.value, ySize.value
        };
        return size;
    }

    public boolean expSetup(long exposureTime, short x0, short x1, short y0, short y1, short xbin, short ybin)
    {
        boolean status = camera.expSetup(exposureTime, x0, x1, y0, y1, xbin, ybin);
        return status;
    }

    public boolean expStart()
    {
        boolean status = camera.expStart();
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
            return f_op_exp.get();
        }
        return null;
    }

    public boolean expHalt(short camState)
    {
        boolean status = camera.expHalt(camState);
        image.value = null;
        f_op_exp = null;
        imageQueue.clear();
        return status;
    }

    public boolean contStart(long exposureTime, short x0, short x1, short y0, short y1, short xbin, short ybin)
    {
        boolean status = camera.contStart(exposureTime, x0, x1, y0, y1, xbin, ybin);
        if (status)
        {
            contStartFlag = true;
            f_op = exec.submit(new GrabImagesContiniously());
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
        boolean status = camera.contHalt(camState);
        image.value = null;
        imageQueue.clear();
        return status;
    }

    public byte[] contGetFrame() throws IOException
    {
        byte[] img = null;
        if (timeoutFlag)
        {
            throw new IOException("Capture thread has timed out.");
        }
        lastImageGrabTime = System.currentTimeMillis();
        if (!imageQueue.isEmpty())
        {
            img = (byte[]) imageQueue.pollLast();
            return img;
        }
        return null;
    }

    public String getError()
    {
        StringHolder error = new StringHolder();
        camera.getError(error);
        return error.value;
    }

    public String[] getParamList()
    {
        ParamContainerHolder params = new ParamContainerHolder();
        camera.getParamList(params);
        return params.value;
    }

    public int getCcsStatus()
    {
        IntHolder ccsStatus = new IntHolder();
        if (!camera.getCcsStatus(ccsStatus))
        {
            return -999;
        }
        return ccsStatus.value;
    }

    public short getShtrStatus()
    {
        ShortHolder shtrStatus = new ShortHolder();
        if (!camera.getShtrStatus(shtrStatus))
        {
            return -999;
        }
        return shtrStatus.value;
    }

    public boolean shtrOpenNever()
    {
        boolean status = camera.shtrOpenNever();
        return status;
    }

    public boolean shtrOpenNormal()
    {
        boolean status = camera.shtrOpenNormal();
        return status;
    }

    public int getShtrOpenDly()
    {
        IntHolder shtrOpenDly = new IntHolder();
        if (!camera.getShtrOpenDly(shtrOpenDly))
        {
            return -999;
        }
        return shtrOpenDly.value;
    }

    public boolean setShtrOpenDly(int shtrOpenDly)
    {
        boolean status = camera.setShtrOpenDly(shtrOpenDly);
        return status;
    }

    public int getShtrCloseDly()
    {
        IntHolder shtrCloseDly = new IntHolder();
        if (!camera.getShtrCloseDly(shtrCloseDly))
        {
            return -999;
        }
        return shtrCloseDly.value;
    }

    public boolean setShtrCloseDly(int shtrCloseDly)
    {
        boolean status = camera.setShtrCloseDly(shtrCloseDly);
        return status;
    }

    public int getClearCycles()
    {
        IntHolder cycles = new IntHolder();
        if (!camera.getClearCycles(cycles))
        {
            return -999;
        }
        return cycles.value;
    }

    public boolean setClearCycles(int clearCycles)
    {
        boolean status = camera.setClearCycles(clearCycles);
        return status;
    }

    public int getClearStrips()
    {
        IntHolder strips = new IntHolder();
        if (!camera.getClearStrips(strips))
        {
            return -999;
        }
        return strips.value;
    }

    public boolean setClearStrips(int strips)
    {
        boolean status = camera.setClearStrips(strips);
        return status;
    }

    public float getTmp()
    {
        FloatHolder curTmp = new FloatHolder();
        if (!camera.getTmp(curTmp))
        {
            return -999.0f;
        }
        return curTmp.value;
    }

    public float getTmpSetpoint()
    {
        FloatHolder tmpSetpoint = new FloatHolder();
        if (!camera.getTmpSetpoint(tmpSetpoint))
        {
            return -999.0f;
        }
        return tmpSetpoint.value;
    }

    public boolean setTmpSetpoint(float tmpSetpoint)
    {
        boolean status = camera.setTmpSetpoint(tmpSetpoint);
        return status;
    }

    public int getMaxGain()
    {
        IntHolder maxGain = new IntHolder();
        if (!camera.getMaxGain(maxGain))
        {
            return -999;
        }
        return maxGain.value;
    }

    public int getGain()
    {
        IntHolder gain = new IntHolder();
        boolean status = camera.getGain(gain);
        return gain.value;
    }

    public boolean setGain(int gain)
    {
        boolean status = camera.setGain(gain);
        return status;
    }

    public int getGainMultEnable()
    {
        IntHolder enabled = new IntHolder();
        if (!camera.getGainMultEnable(enabled))
        {
            return -999;
        }
        return enabled.value;
    }

    public boolean setGainMultEnable(int enabled)
    {
        boolean status = camera.setGainMultEnable(enabled);
        return status;
    }

    public int getGainMultFactor()
    {
        IntHolder factor = new IntHolder();
        if (!camera.getGainMultFactor(factor))
        {
            return -999;
        }
        return factor.value;
    }

    public boolean setGainMultFactor(int factor)
    {
        boolean status = camera.setGainMultFactor(factor);
        return status;
    }

    public int getSpeed()
    {
        IntHolder speed = new IntHolder();
        if (!camera.getSpeed(speed))
        {
            return -999;
        }
        return speed.value;
    }

    public int getSpeedEntries()
    {
        IntHolder entries = new IntHolder();
        if (!camera.getSpeedEntries(entries))
        {
            return -999;
        }
        return entries.value;
    }

    public int getReadoutPortEntries()
    {
        IntHolder entries = new IntHolder();
        if (!camera.getReadoutPortEntries(entries))
        {
            return -999;
        }
        return entries.value;
    }

    public int getBits()
    {
        IntHolder bits = new IntHolder();
        if (!camera.getBits(bits))
        {
            return -999;
        }
        return bits.value;
    }

    public short getSpeedMode()
    {
        ShortHolder speed = new ShortHolder();
        if (!camera.getSpeedMode(speed))
        {
            return -999;
        }
        return speed.value;
    }

    public boolean setSpeedMode(short speed)
    {
        boolean status = camera.setSpeedMode(speed);
        return status;
    }

    public short getReadoutPort()
    {
        ShortHolder port = new ShortHolder();
        if (!camera.getReadoutPort(port))
        {
            return -999;
        }
        return port.value;
    }

    public boolean setReadoutport(short port)
    {
        boolean status = camera.setReadoutport(port);
        return status;
    }

    public int getRetries()
    {
        IntHolder retries = new IntHolder();
        if (!camera.getRetries(retries))
        {
            return -999;
        }
        return retries.value;
    }

    public boolean setRetries(int retries)
    {
        boolean status = camera.setRetries(retries);
        return status;
    }

    public int getTimeout()
    {
        IntHolder mSec = new IntHolder();
        if (!camera.getTimeout(mSec))
        {
            return -999;
        }
        return mSec.value;
    }

    public boolean setTimeout(int mSec)
    {
        boolean status = camera.setTimeout(mSec);
        return status;
    }

    public int getFrameCapable()
    {
        IntHolder frameCapable = new IntHolder();
        if (!camera.getFrameCapable(frameCapable))
        {
            return -999;
        }
        return frameCapable.value;
    }

    /**
     * Grabs images using blocking get and pushes to a fifo circular buffer.
     */
    private class GrabImagesContiniously implements Runnable
    {

        public GrabImagesContiniously()
        {
            timeoutFlag = false;
            lastImageGrabTime = System.currentTimeMillis();
        }

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
                    if (camera.contCheckStatus())
                    {
                        camera.contGetFrame(image);
                        if (!imageQueue.isEmpty())
                        {
                            imageQueue.removeLast();
                        }
                        imageQueue.push(image.value);
                    }
                    Thread.sleep(10);
                }
            }
            catch (InterruptedException ex)
            {
                //From Thread.sleep
                //Ignore interrupted sleep
            }
            catch (CamException ex)
            {
                imageQueue.clear();
                timeoutFlag = true;
                contStartFlag = false;
            }
        }
    }

    /**
     * Grabs an image from the camera in the background.
     */
    private class GrabSingleImage implements Callable
    {

        private ImageContainerHolder singleImage;

        public GrabSingleImage()
        {
            singleImage = new ImageContainerHolder();
        }

        public byte[] call()
        {
            try
            {
                while (!camera.expCheckStatus())
                {
                    Thread.sleep(20);
                }
                if (!camera.expGetFrame(singleImage))
                {
                    return null;
                }
            }
            catch (InterruptedException ex)
            {
                //From Thread.sleep
                //Ignore interrupted sleep
            }
            return singleImage.value;
        }
    }

    protected void finalize()
    {
        //camera.stopCapture();
        //camera.uninitCamera(cameraIndex);
        imageQueue.clear();
        if (ic != null)
        {
            // Clean up
            //
            try
            {
                ic.destroy();
            }
            catch (Exception e)
            {
                //Ignore
                //System.err.println(e.getMessage());
            }
        }
    }
}
