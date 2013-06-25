CXRO's Pixis Library drivers
=================================================================

This contains libraries and source code to operate the PI Pixis camera.

Contents

	./SDK
	Contains the SDK, libraries and includes for both Windows and Linux.
	
	./cpp
	Various C++ projects and wrappers. All projects are available in the VS2008 solution Pixis.sln
		
		Pixis/
		The primary C++ wrapper for PVCAM. This simplifies the interface and adds extra security.
		Netbeans 7.3.1 project here as well.
		
		Pixis_JNI/
		A native interface to allow Java to call Pixis.
		Netbeans 7.3.1 project here as well.
		
		PixisTest/
		A local test of the Pixis library.
		Netbeans 7.3.1 project here as well.
	
	./java
	Contains necessary interfaces for Java to speak to the Pixis. Projects are Netbeans 7.3.1
	
		Pixis/
		Utilises Pixis_JNI.dll to interface to the Pixis locally. Also has added security and
		capturing threads to ensure the camera's circular buffer does not eat its tail.
		
		PixisTest/
		A local test of the Pixis library through JNI.
		
Requirements

	pvcam-pilk from http://github.com/delmic/pvcam-pilk (also included in ./SDK)
	Pvcam library - version 2.7.1.6 or later required
	Download from ftp site: ftp://ftp.piacton.com/Public/Software/Official/Linux/
	This driver has been tested to work on Linux kernel 3.2 and 3.5, on x86 32-bit.
	It is not supported on x86 64-bit.
	
Instructions

	The directory