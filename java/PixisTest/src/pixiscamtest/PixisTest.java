/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package pixiscamtest;

import cxro.common.device.Pixis.Pixis;
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
public class PixisTest
{

    public static RawMaker rawMaker = new RawMaker();
    public static Pixis camera;
    public static byte[] image;

    static void testCapture(String filename, short binning) throws IOException, InterruptedException, ExecutionException
    {
        System.out.println("Capturing single image");
        short[] size = camera.getSensorSize();
        short[] bin =
        {
            binning, binning
        };
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
        System.out.println(String.format("Saving (%dpx,%dpx : 16-bit) to: " + filename, size[0] / bin[0], size[1] / bin[1]));
        RawMaker.createSingleImage(filename, image);
        System.out.println("Done.");
        System.out.println("Stopping capture.");
        if (!camera.expHalt((short) 2))
        {
            System.out.println("FAILED TO HALT DEVICE.");
        }
        System.out.println("Done.");
    }

    static void testCont(String filename, int numImages, short binning) throws IOException, InterruptedException
    {
        RawMaker maker = new RawMaker();
        maker.startImageSequence(filename);
        System.out.println(String.format("Capturing %d images continuously, binning %d.", numImages, binning));
        short[] size = camera.getSensorSize();
        short[] bin =
        {
            binning, binning
        };
        if (!camera.contStart((long) 50, (short) 0, (short) (size[0] - 1), (short) 0, (short) (size[1] - 1), bin[0], bin[1]))
        {
            throw new IOException("Setup/Start Failure: " + camera.getError());
        }
        System.out.println("Starting...");
        for (int x = 1; x < numImages + 1; x++)
        {
//            if( (x%10) == 0)
//            {
//                System.out.println("Debug. Wait 30s");
//                Thread.sleep(30000);
//            }
            System.out.print(String.format("Image #%d...", x));
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
        System.out.println(String.format("%d Images (%dpx,%dpx : 16-bit) Saved to: " + filename, numImages, size[0] / bin[0], size[1] / bin[1]));
        System.out.println("Stopping capture.");
        if (!camera.contHalt((short) 2))
        {
            System.out.println("FAILED TO HALT THE DEVICE.");
        }
        System.out.println("Done.");

    }

    static void testHurt() throws InterruptedException, IOException, ExecutionException
    {
        System.out.println("Hurting things.");

        System.out.println("Capturing a sequence of images.");

        testCont("hurtSequence1.raw", 10, (short) 1);

        System.out.println("Capturing a single image.");

        testCapture("hurtSingle.raw", (short) 2);

        System.out.println("Capturing a sequence of images.");

        testCont("hurtSequence2.raw", 10, (short) 4);

    }

    public static void main(String[] args) throws IOException, InterruptedException
    {
        BufferedReader consoleRead = new BufferedReader(new InputStreamReader(System.in));
        System.out.println("Starting Camera JNI.");
        camera = new Pixis();
        System.out.println("Connecting to camera 0.");
        if (!camera.initCamera(0))
        {
            throw new IOException("Camera Failed to Init: " + camera.getError());
        }
//            System.out.println("Getting/Printing Param List");
//            String[] paramList = camera.getParamList();
//            for(int x = 0; x < paramList.length; x++)
//            {
//                System.out.println(paramList[x]);
//            }
        System.out.println("Printing params and their values:");
        try
        {
            System.out.println("getBits() : " + camera.getBits());
        } catch (IOException ex)
        {
            System.out.println("Failed on getBits(): " + ex);
        }
        try
        {
            System.out.println("getCcsStatus() : " + camera.getCcsStatus());
        } catch (IOException ex)
        {
            System.out.println("Failed on getCcsStatus(): " + ex);
        }
        try
        {
            System.out.println("getClearCycles() : " + camera.getClearCycles());
        } catch (IOException ex)
        {
            System.out.println("Failed on getClearCycles(): " + ex);
        }
        try
        {
            System.out.println("getClearStrips() : " + camera.getClearStrips());
        } catch (IOException ex)
        {
            System.out.println("Failed on getClearStrips(): " + ex);
        }
        try
        {
            System.out.println("getFrameCapable() : " + camera.getFrameCapable());
        } catch (IOException ex)
        {
            System.out.println("Failed on getFrameCapable(): " + ex);
        }
        try
        {
            System.out.println("getGain() : " + camera.getGain());
        } catch (IOException ex)
        {
            System.out.println("Failed on getGain(): " + ex);
        }
        try
        {
            System.out.println("getGainMultEnable() : " + camera.getGainMultEnable());
        } catch (IOException ex)
        {
            System.out.println("Failed on getGainMultEnable(): " + ex);
        }
        try
        {
            System.out.println("getGainMultFactor() : " + camera.getGainMultFactor());
        } catch (IOException ex)
        {
            System.out.println("Failed on getGainMultFactor(): " + ex);
        }
        try
        {
            System.out.println("getMaxGain() : " + camera.getMaxGain());
        } catch (IOException ex)
        {
            System.out.println("Failed on getMaxGain(): " + ex);
        }
        try
        {
            System.out.println("getReadoutPort() : " + camera.getReadoutPort());
        } catch (IOException ex)
        {
            System.out.println("Failed on getReadoutPort(): " + ex);
        }
        try
        {
            System.out.println("getReadoutPortEntries() : " + camera.getReadoutPortEntries());
        } catch (IOException ex)
        {
            System.out.println("Failed on getReadoutPortEntries(): " + ex);
        }
        try
        {
            System.out.println("getRetries() : " + camera.getRetries());
        } catch (IOException ex)
        {
            System.out.println("Failed on getRetries(): " + ex);
        }
        try
        {
            System.out.println("getShtrCloseDly() : " + camera.getShtrCloseDly());
        } catch (IOException ex)
        {
            System.out.println("Failed on getShtrCloseDly(): " + ex);
        }
        try
        {
            System.out.println("getShtrOpenDly() : " + camera.getShtrOpenDly());
        } catch (IOException ex)
        {
            System.out.println("Failed on getShtrOpenDly(): " + ex);
        }
        try
        {
            System.out.println("getShtrStatus() : " + camera.getShtrStatus());
        } catch (IOException ex)
        {
            System.out.println("Failed on getShtrStatus(): " + ex);
        }
        try
        {
            System.out.println("getSpeed() : " + camera.getSpeed());
        } catch (IOException ex)
        {
            System.out.println("Failed on getSpeed(): " + ex);
        }
        try
        {
            System.out.println("getSpeedEntries() : " + camera.getSpeedEntries());
        } catch (IOException ex)
        {
            System.out.println("Failed on getSpeedEntries(): " + ex);
        }
        try
        {
            System.out.println("getSpeedMode() : " + camera.getSpeedMode());
        } catch (IOException ex)
        {
            System.out.println("Failed on getSpeedMode(): " + ex);
        }
        try
        {
            System.out.println("getTimeout() : " + camera.getTimeout());
        } catch (IOException ex)
        {
            System.out.println("Failed on getTimeout(): " + ex);
        }
        try
        {
            System.out.println("getTmp() : " + camera.getTmp());
        } catch (IOException ex)
        {
            System.out.println("Failed on getTmp(): " + ex);
        }
        try
        {
            System.out.println("getTmpSetpoint() : " + camera.getTmpSetpoint());
        } catch (IOException ex)
        {
            System.out.println("Failed on getTmpSetpoint(): " + ex);
        }
        char mode = 0;

        while (mode != 'q')
        {
            System.out.print("\nChoose one:\n"
                    + "1. Get single image.\n"
                    + "2. Get images (Continuous)\n"
                    + "3. Hammer device/library\n"
                    + "4. Hammer device/library(Extensive)\n"
                    + "q. Quit.\n"
                    + "->");
            mode = consoleRead.readLine().charAt(0);

            try
            {
                switch (mode)
                {
                    case '1':
                        testCapture("singleImage.raw", (short) 1);
                        break;
                    case '2':
                        testCont("contImage.raw", 20, (short) 1);
                        break;
                    case '3':
                        testHurt();
                        break;
                    case '4':
                        for(int x = 0; x < 50; x++)
                        {
                            System.out.println("Iteration " + x);
                            testHurt();
                        }
                        break;
                }
            } catch (Exception ex)
            {
                System.err.println(ex);
            }
        }
        camera.uninitCamera();
        System.exit(0);
    }
}
