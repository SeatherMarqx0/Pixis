/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package cxro.common.device.Pixis;

import Ice.Current;
import cxro.common.device.Pixis.CamException;
import cxro.common.device.Pixis.ImageContainerHolder;
import cxro.common.device.Pixis.ParamContainerHolder;
import java.io.IOException;
import java.util.concurrent.ExecutionException;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author wcork
 */

public final class PixisServant extends _PixisICEDisp
{
    private final cxro.common.device.Pixis.Pixis pixis;
    
    public PixisServant(cxro.common.device.Pixis.Pixis pixis)
    {
        this.pixis = pixis;
    }

    @Override
    public boolean
    contCheckStatus(Ice.Current __current) throws CamException
    {
        try
        {
            return pixis.contCheckStatus();
        }
        catch (IOException ex)
        {
            throw new CamException(ex);
        }
    }

    @Override
    public boolean
    contGetFrame(ImageContainerHolder image, Ice.Current __current)
    {
        try
        {
            image.value = pixis.contGetFrame();
            if (image.value == null)
            {
                return false;
            }
            else
            {
                return true;
            }
        }
        catch (IOException ex)
        {
            Logger.getLogger(PixisServant.class.getName()).log(Level.SEVERE, null, ex);
            return false;
        }
    }

    @Override
    public boolean
    contHalt(short camState, Ice.Current __current)
    {
        return pixis.contHalt(camState);
    }

    @Override
    public boolean
    contStart(long exposureTime, short x0, short x1, short y0, short y1, short xbin, short ybin, Ice.Current __current)
    {
        return pixis.contStart(exposureTime, x0, x1, y0, y1, xbin, ybin);
    }

    @Override
    public boolean
    expCheckStatus(Ice.Current __current)
    {
        return pixis.expCheckStatus();
    }

    @Override
    public boolean
    expGetFrame(ImageContainerHolder image, Ice.Current __current)
    {
        try
        {
            image.value = pixis.expGetFrame();
            if (image.value == null)
            {
                return false;
            }
            else
            {
                return true;
            }
        }
        catch (InterruptedException | ExecutionException ex)
        {
            Logger.getLogger(PixisServant.class.getName()).log(Level.SEVERE, null, ex);
            return false;
        }
    }

    @Override
    public boolean
    expHalt(short camState, Ice.Current __current)
    {
        return pixis.expHalt(camState);
    }

    @Override
    public boolean
    expSetup(long exposureTime, short x0, short x1, short y0, short y1, short xbin, short ybin, Ice.Current __current)
    {
        return pixis.expSetup(exposureTime, x0, x1, y0, y1, xbin, ybin);
    }

    @Override
    public boolean
    expStart(Ice.Current __current)
    {
        return pixis.expStart();
    }

    @Override
    public boolean
    getBits(Ice.IntHolder bits, Ice.Current __current)
    {
        try
        {
            bits.value = pixis.getBits();
            return true;
        }
        catch (IOException ex)
        {
            Logger.getLogger(PixisServant.class.getName()).log(Level.SEVERE, null, ex);
            return false;
        }
    }

    @Override
    public boolean
    getCcsStatus(Ice.IntHolder ccsStatus, Ice.Current __current)
    {
        try
        {
            ccsStatus.value = pixis.getCcsStatus();
            return true;
        }
        catch (IOException ex)
        {
            Logger.getLogger(PixisServant.class.getName()).log(Level.SEVERE, null, ex);
            return false;
        }
    }

    @Override
    public boolean
    getClearCycles(Ice.IntHolder cycles, Ice.Current __current)
    {
        try
        {
            cycles.value = pixis.getClearCycles();
            return true;
        }
        catch (IOException ex)
        {
            Logger.getLogger(PixisServant.class.getName()).log(Level.SEVERE, null, ex);
            return false;
        }
    }

    @Override
    public boolean
    getClearStrips(Ice.IntHolder strips, Ice.Current __current)
    {
        try
        {
            strips.value = pixis.getClearStrips();
            return true;
        }
        catch (IOException ex)
        {
            Logger.getLogger(PixisServant.class.getName()).log(Level.SEVERE, null, ex);
            return false;
        }
    }

    @Override
    public boolean
    getError(Ice.StringHolder error, Ice.Current __current)
    {
        error.value = pixis.getError();
        return true;
    }

    @Override
    public boolean
    getFrameCapable(Ice.IntHolder frameCapable, Ice.Current __current)
    {
        try
        {
            frameCapable.value = pixis.getFrameCapable();
            return true;
        }
        catch (IOException ex)
        {
            Logger.getLogger(PixisServant.class.getName()).log(Level.SEVERE, null, ex);
            return false;
        }
    }

    @Override
    public boolean
    getGain(Ice.IntHolder gain, Ice.Current __current)
    {
        try
        {
            gain.value = pixis.getGain();
            return true;
        }
        catch (IOException ex)
        {
            Logger.getLogger(PixisServant.class.getName()).log(Level.SEVERE, null, ex);
            return false;
        }
    }

    @Override
    public boolean
    getGainMultEnable(Ice.IntHolder enabled, Ice.Current __current)
    {
        try
        {
            enabled.value = pixis.getGainMultEnable();
            return true;
        }
        catch (IOException ex)
        {
            Logger.getLogger(PixisServant.class.getName()).log(Level.SEVERE, null, ex);
            return false;
        }
    }

    @Override
    public boolean
    getGainMultFactor(Ice.IntHolder factor, Ice.Current __current)
    {
        try
        {
            factor.value = pixis.getGainMultFactor();
            return true;
        }
        catch (IOException ex)
        {
            Logger.getLogger(PixisServant.class.getName()).log(Level.SEVERE, null, ex);
            return false;
        }
    }

    @Override
    public boolean
    getMaxGain(Ice.IntHolder maxGain, Ice.Current __current)
    {
        try
        {
            maxGain.value = pixis.getMaxGain();
            return true;
        }
        catch (IOException ex)
        {
            Logger.getLogger(PixisServant.class.getName()).log(Level.SEVERE, null, ex);
            return false;
        }
    }

    @Override
    public boolean
    getParamList(ParamContainerHolder params, Ice.Current __current)
    {
        params.value = pixis.getParamList();
        return true;
    }

    @Override
    public boolean
    getReadoutPort(Ice.ShortHolder port, Ice.Current __current)
    {
        try
        {
            port.value = pixis.getReadoutPort();
            return true;
        }
        catch (IOException ex)
        {
            Logger.getLogger(PixisServant.class.getName()).log(Level.SEVERE, null, ex);
            return false;
        }
    }

    @Override
    public boolean
    getReadoutPortEntries(Ice.IntHolder entries, Ice.Current __current)
    {
        try
        {
            entries.value = pixis.getReadoutPortEntries();
            return true;
        }
        catch (IOException ex)
        {
            Logger.getLogger(PixisServant.class.getName()).log(Level.SEVERE, null, ex);
            return false;
        }
    }

    @Override
    public boolean
    getRetries(Ice.IntHolder retries, Ice.Current __current)
    {
        try
        {
            retries.value = pixis.getRetries();
            return true;
        }
        catch (IOException ex)
        {
            Logger.getLogger(PixisServant.class.getName()).log(Level.SEVERE, null, ex);
            return false;
        }
    }

    @Override
    public boolean
    getSensorSize(Ice.ShortHolder xSize, Ice.ShortHolder ySize, Ice.Current __current)
    {
        xSize.value = pixis.getSensorSize()[0];
        ySize.value = pixis.getSensorSize()[1];
        return true;
    }

    @Override
    public boolean
    getShtrCloseDly(Ice.IntHolder shtrCloseDly, Ice.Current __current)
    {
        try
        {
            shtrCloseDly.value = pixis.getShtrCloseDly();
            return true;
        }
        catch (IOException ex)
        {
            Logger.getLogger(PixisServant.class.getName()).log(Level.SEVERE, null, ex);
            return false;
        }
    }

    @Override
    public boolean
    getShtrOpenDly(Ice.IntHolder shtrOpenDly, Ice.Current __current)
    {
        try
        {
            shtrOpenDly.value = pixis.getShtrOpenDly();
            return true;
        }
        catch (IOException ex)
        {
            Logger.getLogger(PixisServant.class.getName()).log(Level.SEVERE, null, ex);
            return false;
        }
    }

    @Override
    public boolean
    getShtrStatus(Ice.ShortHolder shtrStatus, Ice.Current __current)
    {
        try
        {
            shtrStatus.value = pixis.getShtrStatus();
            return true;
        }
        catch (IOException ex)
        {
            Logger.getLogger(PixisServant.class.getName()).log(Level.SEVERE, null, ex);
            return false;
        }
    }

    @Override
    public boolean
    getSpeed(Ice.IntHolder speed, Ice.Current __current)
    {
        try
        {
            speed.value = pixis.getSpeed();
            return true;
        }
        catch (IOException ex)
        {
            Logger.getLogger(PixisServant.class.getName()).log(Level.SEVERE, null, ex);
            return false;
        }
    }

    @Override
    public boolean
    getSpeedEntries(Ice.IntHolder entries, Ice.Current __current)
    {
        try
        {
            entries.value = pixis.getSpeedEntries();
            return true;
        }
        catch (IOException ex)
        {
            Logger.getLogger(PixisServant.class.getName()).log(Level.SEVERE, null, ex);
            return false;
        }
    }

    @Override
    public boolean
    getSpeedMode(Ice.ShortHolder speed, Ice.Current __current)
    {
        try
        {
            speed.value = pixis.getSpeedMode();
            return true;
        }
        catch (IOException ex)
        {
            Logger.getLogger(PixisServant.class.getName()).log(Level.SEVERE, null, ex);
            return false;
        }
    }

    @Override
    public boolean
    getTimeout(Ice.IntHolder mSec, Ice.Current __current)
    {
        try
        {
            mSec.value = pixis.getTimeout();
            return true;
        }
        catch (IOException ex)
        {
            Logger.getLogger(PixisServant.class.getName()).log(Level.SEVERE, null, ex);
            return false;
        }
    }

    @Override
    public boolean
    getTmp(Ice.FloatHolder curTmp, Ice.Current __current)
    {
        try
        {
            curTmp.value = pixis.getTmp();
            return true;
        }
        catch (IOException ex)
        {
            Logger.getLogger(PixisServant.class.getName()).log(Level.SEVERE, null, ex);
            return false;
        }
    }

    @Override
    public boolean
    getTmpSetpoint(Ice.FloatHolder tmpSetpoint, Ice.Current __current)
    {
        try
        {
            tmpSetpoint.value = pixis.getTmpSetpoint();
            return true;
        }
        catch (IOException ex)
        {
            Logger.getLogger(PixisServant.class.getName()).log(Level.SEVERE, null, ex);
            return false;
        }
    }

    @Override
    public boolean
    initCamera(int index, Ice.Current __current)
        throws CamException
    {
        return pixis.initCamera(index);
    }

    @Override
    public boolean
    setClearCycles(int clearCycles, Ice.Current __current)
    {
        return pixis.setClearCycles(clearCycles);
    }

    @Override
    public boolean
    setClearStrips(int strips, Ice.Current __current)
    {
        return pixis.setClearStrips(strips);
    }

    @Override
    public boolean
    setGain(int gain, Ice.Current __current)
    {
        return pixis.setGain(gain);
    }

    @Override
    public boolean
    setGainMultEnable(int enabled, Ice.Current __current)
    {
        return pixis.setGainMultEnable(enabled);
    }

    @Override
    public boolean
    setGainMultFactor(int factor, Ice.Current __current)
    {
        return pixis.setGainMultFactor(factor);
    }

    @Override
    public boolean
    setReadoutport(short port, Ice.Current __current)
    {
        return pixis.setReadoutport(port);
    }

    @Override
    public boolean
    setRetries(int retries, Ice.Current __current)
    {
        return pixis.setRetries(retries);
    }

    @Override
    public boolean
    setShtrCloseDly(int shtrCloseDly, Ice.Current __current)
    {
        return pixis.setShtrCloseDly(shtrCloseDly);
    }

    @Override
    public boolean
    setShtrOpenDly(int shtrOpenDly, Ice.Current __current)
    {
        return pixis.setShtrOpenDly(shtrOpenDly);
    }

    @Override
    public boolean
    setSpeedMode(short speed, Ice.Current __current)
    {
        return pixis.setSpeedMode(speed);
    }

    @Override
    public boolean
    setTimeout(int mSec, Ice.Current __current)
    {
        return pixis.setTimeout(mSec);
    }

    @Override
    public boolean
    setTmpSetpoint(float tmpSetpoint, Ice.Current __current)
    {
        return pixis.setTmpSetpoint(tmpSetpoint);
    }

    @Override
    public boolean
    shtrOpenNever(Ice.Current __current)
    {
        return pixis.shtrOpenNever();
    }

    @Override
    public boolean
    shtrOpenNormal(Ice.Current __current)
    {
        return pixis.shtrOpenNormal();
    }

    @Override
    public boolean
    uninitCamera(Ice.Current __current)
    {
        return pixis.uninitCamera();
    }
}