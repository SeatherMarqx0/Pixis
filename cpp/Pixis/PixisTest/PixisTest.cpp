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
#include "BMPMaker.h"
#include "RAWMaker.h"

#ifdef linux
#include <unistd.h>
#define TIMEMULT 10
void Sleep(int ms)
{
    usleep(ms * 1000);
}
#else
#define TIMEMULT 1
void Sleep(int ms)
{
    _sleep(ms);
}
#endif

using namespace std;

Pixis *camera;

bool singleImage(const char* filename, unsigned short binning)
{
    /* Global Memory Pointers */
    unsigned short *buffer;
    clock_t t1, t2;
    double totalTime = 0.0;
    cout << "Setting up Camera for single Image:\n";

    unsigned short xdimension;
    unsigned short ydimension;
    /* Setup a region of interest */
    camera->get_sensor_size(&xdimension, &ydimension);

    /* Region Of Interest, Layout */
    unsigned short SB = binning; // Serial Binning
    unsigned short PB = binning; // Parallel

    unsigned short S1 = 0; // Serial Start
    unsigned short S2 = (unsigned short) (xdimension - 1); // Serial Stop
    unsigned short P1 = 0; // Parallel Start
    unsigned short P2 = (unsigned short) (ydimension - 1); // Parallel Stop

    long npixels = ((S2 - S1 + 1) / SB) * ((P2 - P1 + 1) / PB);
    unsigned long stream_size;

    printf("Sensor size         : %d,%d\n", xdimension, ydimension);
    printf("Region of interest  : s(%d,%d)->p(%d,%d)\n", S1, S2, P1, P2);
    printf("Binning             : s(%d),p(%d)\n", SB, PB);
    printf("Number of Pixels    : %d = (%d,%d)\n", npixels, (S2 - S1 + 1) / SB, (P2 - P1 + 1) / PB);
    
    if (!camera->exp_setup(500, S1, S2, P1, P2, SB, PB, &stream_size))
    {
        cout << "Something bad (Could not Setup) : ";
        string err;
        camera->get_error(err);
        cout << err << endl;
        return false;
    } else
    {
        cout << "Camera setup.\n";
    }

    printf("\nAllocating memory   : %d bytes\n", stream_size);
    t1 = clock();
    //Allocate memory for image
    buffer = (unsigned short *) malloc(stream_size);
    t2 = clock();
    totalTime = (double) (t2 - t1) * TIMEMULT / CLOCKS_PER_SEC;
    printf("Done. (%.2fs)\n", totalTime);

    cout << "Getting image\n";
    t1 = clock();
    if (!camera->exp_start(buffer))
    {
        cout << "Something bad. Could not get image.\n";
        return false;
    }
    
    short status = 0;
    unsigned long numBytes = 0;
    do
    {
        //cout << "Status: " << status << endl;
        camera->exp_check_status(&status, &numBytes);
    } while (!(status == 3));

    t2 = clock();
    totalTime = (double) (t2 - t1) * TIMEMULT / CLOCKS_PER_SEC;
    printf("Done. (%.2fs)\n", totalTime);

    cout << "Output to file\n";
    t1 = clock();
    RAWMaker::createSingleImage(filename, (S2 - S1 + 1) / SB, (P2 - P1 + 1) / PB, buffer, 2);
    t2 = clock();
    totalTime = (double) (t2 - t1) * TIMEMULT / CLOCKS_PER_SEC;
    printf("Done. (%.2fs)\n", totalTime);
    camera->exp_halt(2);
    
    return true;
}

bool sequenceImages(const char* filename, int numImages, unsigned short binning)
{

    /* Global Memory Pointers */
    unsigned short *buffer;
    clock_t t1, t2;
    double totalTime = 0.0;


    cout << "Setting up Camera for sequence of " << numImages << " images, binning " << binning << endl;

    unsigned short xdimension;
    unsigned short ydimension;
    /* Setup a region of interest */
    camera->get_sensor_size(&xdimension, &ydimension);

    /* Region Of Interest, Layout */
    unsigned short SB = binning; // Serial Binning
    unsigned short PB = binning; // Parallel

    unsigned short S1 = 0; // Serial Start
    unsigned short S2 = (unsigned short) (xdimension - 1); // Serial Stop
    unsigned short P1 = 0; // Parallel Start
    unsigned short P2 = (unsigned short) (ydimension - 1); // Parallel Stop

    //For testing non-full sensor region

    //S2 = 255;
    //P2 = 512;

    long npixels = ((S2 - S1 + 1) / SB) * ((P2 - P1 + 1) / PB);
    unsigned long stream_size;

    printf("Sensor size         : %d,%d\n", xdimension, ydimension);
    printf("Region of interest  : s(%d,%d)->p(%d,%d)\n", S1, S2, P1, P2);
    printf("Binning             : s(%d),p(%d)\n", SB, PB);
    printf("Number of Pixels    : %d = (%d,%d)\n", npixels, (S2 - S1 + 1) / SB, (P2 - P1 + 1) / PB);

    cout << "\nStarting Image Sequence\n";

    if (!camera->cont_start(50, S1, S2, P1, P2, SB, PB, &stream_size))
    {
        cout << "Something bad. Could not get images.\n";
        return false;
    }

    printf("Allocating memory   : %d bytes\n", stream_size);
    t1 = clock();
    //Allocate memory for image
    buffer = (unsigned short *) malloc(stream_size);
    t2 = clock();
    totalTime = (double) (t2 - t1) * TIMEMULT / CLOCKS_PER_SEC;
    printf("Done. (%.2fs)\n", totalTime);

    short status = 0;
    unsigned long numBytes = 0;
    unsigned long numBuffers = 0;
    int files = 1;
    string error;

    //int sleep = 3000;
    //cout << "Sleeping for " << sleep << "ms\n";
    //_sleep(sleep);

    RAWMaker *maker = new RAWMaker();
    maker->startImageSequence(filename, (S2 - S1 + 1) / SB, (P2 - P1 + 1) / PB, 2);

    while (files < numImages)
    {
        t1 = clock();
        /*
        if( (files%2) == 0)
        {
            cout << "Debug Wait for 6s.\n";
            _sleep(6000);
        }
         */
        printf("Resolving Image #%d Status: ", files);
        do
        {
            Sleep(100);
            camera->cont_check_status(&status, &numBytes, &numBuffers);

            switch (status)
            {
                case 0:
                    cout << status << "\nReadout not active.\n";
                    return false;
                case 3:
                    cout << status << " DONE   ";
                    cout << setw(7) << " Bufr: ";
                    cout << left << setw(6) << numBytes;
                    cout << setw(12) << " FillTimes: ";
                    cout << left << setw(5) << numBuffers << endl;
                    cout << "Readout # " << files << " Complete.\n";
                    break;
                case 4:
                    cout << status << " FAILED ";
                    cout << setw(7) << " Bufr: ";
                    cout << left << setw(6) << numBytes;
                    cout << setw(12) << " FillTimes: ";
                    cout << left << setw(5) << numBuffers << endl;
                    camera->get_error(error);
                    cout << "Camera Error: " << error << endl;
                    return false;
                case 5:
                    cout << left << setw(1) << status;
                    cout << setw(8) << " Reading";
                    cout << setw(7) << " Bufr: ";
                    cout << left << setw(6) << numBytes;
                    cout << setw(12) << " FillTimes: ";
                    cout << left << setw(5) << numBuffers;
                    cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b";
                    break;
            }
        } while (status != 3);
        printf("Exposure done, getting image.\n");
        camera->cont_get_frame(buffer);
        t2 = clock();
        totalTime = (double) (t2 - t1) * TIMEMULT / CLOCKS_PER_SEC;
        printf("Image#%d, Ptr: %p, CircBufr->LocalBuff (%.2fs)\n", files, buffer, totalTime);


        printf("Saving to file\n", files);
        t1 = clock();
        maker->insertImageIntoSequence(buffer);
        t2 = clock();
        totalTime = (double) (t2 - t1) * TIMEMULT / CLOCKS_PER_SEC;
        printf("Done. (%.2fs)\n", totalTime);

        status = 0;
        files++;
    }
    printf("%d Images (%dpx,%dpx : 16-bit) Saved to: %s", numImages, xdimension / SB, ydimension / PB, filename);
    maker->endImageSequence();
    if (!camera->cont_halt(2))
    {
        string err;
        camera->get_error(err);
        cout << "CAMERA HALT FAILED : " << err << endl;
    }
    cout << "Sequence complete.\n";
    return true;
}

bool hammerDevice()
{
    cout << "Hurting things." << endl;

    cout << "Capturing a sequence of images." << endl;
    if (!sequenceImages("hurtSequence1.raw", 10, 1))
    {
        cout << "Test Failed.\n";
        return false;
    }

    cout << "Capturing a single image." << endl;
    if (!singleImage("hurtSingle.raw", 2))
    {
        cout << "Test Failed.\n";
        return false;
    }

    cout << "Capturing a sequence of images." << endl;
    if (!sequenceImages("hurtSequence2.raw", 10, 4))
    {
        cout << "Test Failed.\n";
        return false;
    }

    return true;
}

bool testHoldParam()
{
    float tempSetpointBefore = 0.0;
    float tempSetpointAfter = 0.0;

    if (!camera->get_tmp_setpoint(&tempSetpointBefore))
    {
        cerr << "Couldn't get temperature" << endl;
        return false;
    }

    cout << "Current tempSetpoint: " << tempSetpointBefore << endl;
    tempSetpointBefore -= 10.0;
    cout << "Setting to " << tempSetpointBefore << endl;

    if (!camera->set_tmp_setpoint(tempSetpointBefore))
    {
        cerr << "Couldn't set temperature" << endl;
        return false;
    }

    cout << "Check if worked: ";
    if (!camera->get_tmp_setpoint(&tempSetpointAfter))
    {
        cerr << "Couldn't get temperature" << endl;
        return false;
    }
    if (tempSetpointAfter == tempSetpointBefore)
    {
        cout << "TRUE" << endl;
    } else
    {
        cout << "FALSE" << endl;
        return false;
    }

    if (!singleImage("tempTest.raw", 1))
    {
        cerr << "Couldn't get image" << endl;
        return false;
    }

    if (!camera->get_tmp_setpoint(&tempSetpointAfter))
    {
        cerr << "Couldn't get temperature" << endl;
        return false;
    }

    cout << "tempSetpointBefore: " << tempSetpointBefore << endl;
    cout << "tempSetpointAfter: " << tempSetpointAfter << endl;

    if (tempSetpointBefore != tempSetpointAfter)
    {
        cout << "Camera not maintaining temperature setpoint." << endl;
        return false;
    }

    return true;
}

int main(int argc, char* argv[])
{
    try
    {
        camera = new Pixis(0);
    } catch (std::exception *e)
    {
        cerr << e->what() << endl;
        cin.get();
        return 1;
    }

    //Print Every Parameter
    cout << "Listing Params:" << endl;
    std::vector<string> params;
    short numParams = 0;
    ofstream paramFileList;
    paramFileList.open("paramFileList.txt");
    if (!camera->get_paramList(params, numParams))
    {
        return 1;
    }
    for (int x = 0; x < numParams; x++)
    {
        cout << params[x] << endl;
        paramFileList << params[x] << endl;
    }
    paramFileList.close();

    char mode;
    while (mode != 'q')
    {
        cout << "Testing Pixis" << endl;
        cout << "0. Test holding params.\n"
                << "1. Single Image\n"
                << "2. Sequence of Images.\n"
                << "3. Beat-up library/device.\n"
                << "4. Beat-up library/device (extensive).\n"
                << "5. Sequence of single captures.\n"
                << "6. Sequence of continuous captures.\n"
                << "q. Quit.\n";
        cout << "Choose mode->";
        cin >> mode;

        switch (mode)
        {
            case '0':
            {
                if (!testHoldParam())
                {
                    cout << "Test Failed." << endl;
                }
                break;
            }
            case '1':
            {
                if (!singleImage("testSingle.raw", 1))
                {
                    cout << "Test Failed." << endl;
                }
                break;
            }
            case '2':
            {
                if (!sequenceImages("ImageSequence.raw", 20, 1))
                {
                    cout << "Test Failed." << endl;
                }
                break;
            }
            case '3':
            {
                if (!hammerDevice())
                {
                    cout << "Test Failed." << endl;
                }
                break;
            }
            case '4':
            {
                for (int x = 0; x < 50; x++)
                {
                    cout << "Iteration: " << x << endl;
                    if (!hammerDevice())
                    {
                        cout << "Test Failed." << endl;
                    }
                }
                break;
            }
            case '5':
            {
                for (int x = 0; x < 50; x++)
                {
                    cout << "Iteration: " << x << endl;
                    if (!singleImage("testSingleSequence.raw", 1))
                    {
                        cout << "Test Failed." << endl;
                    }
                }
                break;
            }
            case '6':
            {
                for (int x = 0; x < 50; x++)
                {
                    cout << "Iteration: " << x << endl;
                    if (!sequenceImages("testContinuousSequence.raw", 10, 1))
                    {
                        cout << "Test Failed." << endl;
                    }
                    if (!sequenceImages("testContinuousSequence.raw", 10, 2))
                    {
                        cout << "Test Failed." << endl;
                    }
                    if (!sequenceImages("testContinuousSequence.raw", 10, 4))
                    {
                        cout << "Test Failed." << endl;
                    }
                }
                break;
            }
        }
    }
    cout << endl << "Press any key.";
    delete camera;
    cin.get();
    cin.get();
    return 0;
}