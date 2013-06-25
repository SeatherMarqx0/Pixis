// @license
package cxro.common.device.Pixis;

import java.io.IOException;
import java.util.logging.Level;
import java.util.logging.Logger;


public class PixisServer
    extends Ice.Application
{
    @Override
    public int
    run(String[] args)
    {
        String objectName;
        Pixis pixis;

        if (args.length != 1)
        {
            printUsage();
            return 1;
        }
        else
        {
            try
            {
                objectName = args[0];
            }
            catch (Exception ex)
            {
                printUsage();
                return 1;
            }
        }

        // Initialize pixis
        pixis = new Pixis();

        // Initialize adapter and respond to requests
        Ice.ObjectAdapter adapter;
        adapter = communicator().createObjectAdapter(objectName + "Adapter");
        adapter.add(new PixisServant(pixis), communicator().stringToIdentity(objectName));
        adapter.activate();
        communicator().waitForShutdown();
        return 0;
    }

    public static void
    main(String[] args)
    {
        PixisServer app = new PixisServer();
        int status = app.main("PixisServer", args, "config/PixisServer.config");
        System.exit(status);
    }

    private static void printUsage()
    {
        System.err.println("Usage: " +
            "USAGE: java -jar PixisServer.jar "
            + "<ice objectname [String]>");
        }
}
