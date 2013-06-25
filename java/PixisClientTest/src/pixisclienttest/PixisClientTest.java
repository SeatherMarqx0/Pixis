/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package pixisclienttest;

import cxro.common.device.Pixis.CamException;
import cxro.common.device.PixisClient;
import cxro.common.tools.RawMaker;
import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.concurrent.ExecutionException;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author wcork
 */
public class PixisClientTest
{

    public static RawMaker rawMaker = new RawMaker();
    public static PixisClient camera;
    public static byte[] image;

    static void testCapture(String filename, short binning) throws IOException, InterruptedException, ExecutionException
    {
        System.out.println("Capturing single image");
        short[] size = camera.getSensorSize();
        short[] bin = {binning,binning};
        if (!camera.expSetup((long) 50, (short) 0, (short) (size[0] - 1), (short) 0, (short) (size[1] - 1), bin[0], bin[1]))
        {
            throw new IOException("Setup Failure: " + camera.getError());
        }
        System.out.println("Starting capture");
        if (!camera.expStart())
        {
            throw new IOException("Capture Start Failure");
        }
        System.out.println("Fetching...");
        while (!camera.expCheckStatus())
        {
            Thread.sleep(20);
        }
        System.out.println("Done.");
        image = camera.expGetFrame();
        System.out.println(String.format("Saving (%dpx,%dpx : 16-bit) to: " + filename,size[0]/bin[0],size[1]/bin[1]));
        RawMaker.createSingleImage(filename, image);
        System.out.println("Done.");
        System.out.println("Stopping capture.");
        if(!camera.expHalt((short)2))
        {
            System.out.println("FAILED TO HALD DEVICE.");
        }
        System.out.println("Done.");
    }

    static void testCont(String filename, int numImages, short binning) throws IOException, InterruptedException
    {
        RawMaker maker = new RawMaker();
        maker.startImageSequence(filename);
        System.out.println(String.format("Capturing %d images continuously, binning %d.", numImages, binning));
        short[] size = camera.getSensorSize();
        short[] bin = {binning,binning};
        if (!camera.contStart((long) 50, (short) 0, (short) (size[0] - 1), (short) 0, (short) (size[1] - 1), bin[0], bin[1]))
        {
            throw new IOException("Setup/Start Failure: " + camera.getError());
        }
        System.out.println("Starting...");
        for (int x = 1; x < numImages+1; x++)
        {
//            if( (x%10) == 0)
//            {
//                System.out.println("Debug. Wait 30s");
//                Thread.sleep(30000);
//            }
            System.out.print(String.format("Image #%d...",x));
            while (!camera.contCheckStatus())
            {
                Thread.sleep(20);
            }
            System.out.print("ready...");
            image = camera.contGetFrame();
            System.out.print("received...");
            maker.insertImageIntoSequence(image);
            System.out.println("Done.");
        }
        System.out.println(String.format("%d Images (%dpx,%dpx : 16-bit) Saved to: " + filename, numImages,size[0]/bin[0],size[1]/bin[1]));
        System.out.println("Stopping capture.");
        if(!camera.contHalt((short) 2))
        {
            System.out.println("FAILED TO HALT THE DEVICE.");
        }
        System.out.println("Done.");

    }
    
    static void testHurt() throws InterruptedException, IOException, ExecutionException
    {
        System.out.println("Hurting things.");

        System.out.println("Capturing a sequence of images.");
        
        testCont("hurtSequence1.raw", 10, (short)1);

        System.out.println("Capturing a single image.");
        
        testCapture("hurtSingle.raw", (short)2);

        System.out.println("Capturing a sequence of images.");
        
        testCont("hurtSequence2.raw", 10, (short)4);
        
    }

    public static void main(String[] args) throws IOException, InterruptedException
    {
        try
        {
            BufferedReader consoleRead = new BufferedReader(new InputStreamReader(System.in));
            System.out.println("Starting Camera JNI.");
            camera = new PixisClient();
            System.out.println("Connecting to camera 0.");
            try
            {
                if(!camera.initCamera(0))
                {
                    throw new IOException("Camera Failed to Init: " + camera.getError());
                }
            }
            catch (CamException ex)
            {
                Logger.getLogger(PixisClientTest.class.getName()).log(Level.SEVERE, null, ex);
            }
//            System.out.println("Getting/Printing Param List");
//            String[] paramList = camera.getParamList();
//            for(int x = 0; x < paramList.length; x++)
//            {
//                System.out.println(paramList[x]);
//            }
            System.out.println("Printing params and their values:");
            System.out.println("getBits() : " + camera.getBits());
            System.out.println("getCcsStatus() : " + camera.getCcsStatus());
            System.out.println("getClearCycles() : " + camera.getClearCycles());
            System.out.println("getClearStrips() : " + camera.getClearStrips());
            System.out.println("getFrameCapable() : " + camera.getFrameCapable());
            System.out.println("getGain() : " + camera.getGain());
            System.out.println("getGainMultEnable() : " + camera.getGainMultEnable());
            System.out.println("getGainMultFactor() : " + camera.getGainMultFactor());
            System.out.println("getMaxGain() : " + camera.getMaxGain());
            System.out.println("getReadoutPort() : " + camera.getReadoutPort());
            System.out.println("getReadoutPortEntries() : " + camera.getReadoutPortEntries());
            System.out.println("getRetries() : " + camera.getRetries());
            System.out.println("getShtrCloseDly() : " + camera.getShtrCloseDly());
            System.out.println("getShtrOpenDly() : " + camera.getShtrOpenDly());
            System.out.println("getShtrStatus() : " + camera.getShtrStatus());
            System.out.println("getSpeed() : " + camera.getSpeed());
            System.out.println("getSpeedEntries() : " + camera.getSpeedEntries());
            System.out.println("getSpeedMode() : " + camera.getSpeedMode());
            System.out.println("getTimeout() : " + camera.getTimeout());
            System.out.println("getTmp() : " + camera.getTmp());
            System.out.println("getTmpSetpoint() : " + camera.getTmpSetpoint());

            System.out.print("\nChoose one:\n"
                    + "1. Get single image.\n"
                    + "2. Get images (Continuous)\n"
                    + "3. Hammer device/library\n"
                    + "4. Quit.\n"
                    + "->");
            int mode = Integer.parseInt(consoleRead.readLine());

            switch (mode)
            {
                case 1:
                    testCapture("singleImage.raw",(short)1);
                    break;
                case 2:
                    testCont("contImage.raw",20,(short)1);
                    break;
                case 3:
                    testHurt();
                    break;
                case 4:
                    break;
            }
            camera.uninitCamera();
        }
        catch (ExecutionException ex)
        {
            Logger.getLogger(PixisClientTest.class.getName()).log(Level.SEVERE, null, ex);
        }
        System.exit(0);
    }
}
