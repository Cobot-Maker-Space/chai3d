//==============================================================================
/*
    Software License Agreement (BSD License)
    Copyright (c) 2003-2016, CHAI3D.
    (www.chai3d.org)

    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:

    * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above
    copyright notice, this list of conditions and the following
    disclaimer in the documentation and/or other materials provided
    with the distribution.

    * Neither the name of CHAI3D nor the names of its contributors may
    be used to endorse or promote products derived from this software
    without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
    "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
    LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
    FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
    COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
    INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
    BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
    LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
    ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
    POSSIBILITY OF SUCH DAMAGE.

    \author    <http://www.chai3d.org>
    \author    Your name, institution, or company name.
    \version   3.1.1 $Rev: 1869 $
*/
//==============================================================================


//------------------------------------------------------------------------------
#include "system/CGlobals.h"
#include "devices/CVirtuoseDevice.h"
//------------------------------------------------------------------------------
#if defined(C_ENABLE_CUSTOM_DEVICE_SUPPORT)
//------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/*
    INSTRUCTION TO IMPLEMENT YOUR OWN CUSTOM DEVICE:

    Please review header file CMyCustomDevice.h for some initial
    guidelines about how to implement your own haptic device using this
    template.

    When ready, simply completed the next 11 documented steps described here
    below.
*/
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
namespace chai3d {
//------------------------------------------------------------------------------

// Number of instances for this class of devices currently using the libraries.
unsigned int cVirtuoseDevice::s_libraryCounter = 0;

#if defined(WIN32) | defined(WIN64)
HINSTANCE virtuoseAPI = NULL;

VirtContext (__cdecl *virtOpen)(const char *);
int (__cdecl *virtClose)(VirtContext);
int (__cdecl *virtGetPosition)(VirtContext, float *);
int (__cdecl *virtGetPhysicalPosition)(VirtContext, float *);
int (__cdecl *virtSetForce)(VirtContext, float *);
int (__cdecl *virtGetButton)(VirtContext, int, int *);
int (__cdecl *virtSetCommandType)(VirtContext, VirtCommandType);
int (__cdecl *virtSetPowerOn)(VirtContext, int);
int (__cdecl *virtSetSpeedFactor)(VirtContext, float);
int (__cdecl *virtSetIndexingMode)(VirtContext, VirtIndexingType);
int (__cdecl *virtSetBaseFrame)(VirtContext, float *);

#endif

#define VIRTUOSE_CFG "virtuose.cfg"

//==============================================================================
/*!
    This method opens libraries for this class of devices.

    \return __true__ if the operation succeeds, __false__ otherwise.
*/
//==============================================================================
bool cVirtuoseDevice::openLibraries()
{
    // increment number of instances using the libraries for this class of devices
    s_libraryCounter++;

    // if libraries are already initialized, then we are done
    if (s_libraryCounter > 1) return (C_SUCCESS);

    ////////////////////////////////////////////////////////////////////////////
    // initialize libraries
    ////////////////////////////////////////////////////////////////////////////

#if defined(WIN32) | defined(WIN64)
    #if defined(WIN64)
    // load 64-bit virtuoseAPI library
    if (virtuoseAPI==NULL) virtuoseAPI = LoadLibrary("virtuoseAPI.dll");
    #else
    // load 32-bit virtuoseAPI library
    if (virtuoseAPI==NULL) virtuoseAPI = LoadLibrary("virtuoseAPI.dll");
    #endif
#endif

#if defined(WIN32) | defined(WIN64)

    // check if DLL loaded correctly
    if (virtuoseAPI == NULL)
    {
        s_libraryCounter = 0;
        return (C_ERROR);
    }

    // load different callbacks
    virtOpen = (VirtContext (__cdecl *)(const char *))GetProcAddress(virtuoseAPI, "virtOpen");
    if (virtOpen == NULL) { return (C_ERROR); }

    virtClose = (int (__cdecl*)(VirtContext))GetProcAddress(virtuoseAPI, "virtClose");
    if (virtClose == NULL) { return (C_ERROR); }

    virtGetPosition = (int (__cdecl*)(VirtContext, float *))GetProcAddress(virtuoseAPI, "virtGetPosition");
    if (virtGetPosition == NULL) { return (C_ERROR); }

    virtGetPhysicalPosition = (int (__cdecl*)(VirtContext, float *))GetProcAddress(virtuoseAPI, "virtGetPhysicalPosition");
    if (virtGetPhysicalPosition == NULL) { return (C_ERROR); }

    virtSetForce = (int (__cdecl*)(VirtContext, float *))GetProcAddress(virtuoseAPI, "virtSetForce");
    if (virtSetForce == NULL) { return (C_ERROR); }

    virtGetButton = (int (__cdecl*)(VirtContext, int, int *))GetProcAddress(virtuoseAPI, "virtGetButton");
    if (virtGetButton == NULL) { return (C_ERROR); }

    virtSetCommandType = (int (__cdecl*)(VirtContext, VirtCommandType))GetProcAddress(virtuoseAPI, "virtSetCommandType");
    if (virtSetCommandType == NULL) { return (C_ERROR); }

    virtSetPowerOn = (int (__cdecl*)(VirtContext, int))GetProcAddress(virtuoseAPI, "virtSetPowerOn");
    if (virtSetPowerOn == NULL) { return (C_ERROR); }

    virtSetSpeedFactor = (int (__cdecl*)(VirtContext, float))GetProcAddress(virtuoseAPI, "virtSetSpeedFactor");
    if (virtSetSpeedFactor == NULL) { return (C_ERROR); }

    virtSetIndexingMode = (int (__cdecl*)(VirtContext, VirtIndexingType))GetProcAddress(virtuoseAPI, "virtSetIndexingMode");
    if (virtSetIndexingMode == NULL) { return (C_ERROR); }

    virtSetBaseFrame = (int (__cdecl*)(VirtContext, float *))GetProcAddress(virtuoseAPI, "virtSetBaseFrame");
    if (virtSetBaseFrame == NULL) { return (C_ERROR); }

#endif

    // return success
    return (C_SUCCESS);
}


//==============================================================================
/*!
    This method closes libraries for this class of devices.

    \return __true__ if the operation succeeds, __false__ otherwise.
*/
//==============================================================================
bool cVirtuoseDevice::closeLibraries()
{
    // sanity check
    if (s_libraryCounter < 1) return (C_ERROR);

    // decrement library counter; exit if other objects are still using libraries
    s_libraryCounter--;
    if (s_libraryCounter > 0) return (C_SUCCESS);

    // free libraries
    #if defined(WIN32) | defined(WIN64)
    if ((s_libraryCounter == 0) && (virtuoseAPI != NULL))
    {
        FreeLibrary(virtuoseAPI);
        virtuoseAPI = NULL;
    }
    #endif

    // exit
    return (C_SUCCESS);
}

//==============================================================================
/*!
    Constructor of cMyCustomDevice.
*/
//==============================================================================
cVirtuoseDevice::cVirtuoseDevice(unsigned int a_deviceNumber)
{
    // open libraries
    if (openLibraries() == false)
    {
        return;
    }

    // the connection to your device has not yet been established.
    m_deviceReady = false;

    ////////////////////////////////////////////////////////////////////////////
    /*
        STEP 1:

        Here you should define the specifications of your device.
        These values only need to be estimates. Since haptic devices often perform
        differently depending of their configuration withing their workspace,
        simply use average values.
    */
    ////////////////////////////////////////////////////////////////////////////

    //--------------------------------------------------------------------------
    // NAME:
    //--------------------------------------------------------------------------

    // haptic device model (see file "CGenericHapticDevice.h")
    m_specifications.m_model                         = C_HAPTIC_DEVICE_CUSTOM;

    // name of the device manufacturer, research lab, university.
    m_specifications.m_manufacturerName              = "Haption";

    // name of your device
    m_specifications.m_modelName                     = "Virtuose6D";


    //--------------------------------------------------------------------------
    // CHARACTERISTICS: (The following values must be positive or equal to zero)
    //--------------------------------------------------------------------------

    // the maximum force [N] the device can produce along the x,y,z axis.
    m_specifications.m_maxLinearForce                = 30.0;     // [N]

    // the maximum amount of torque your device can provide arround its
    // rotation degrees of freedom.
    m_specifications.m_maxAngularTorque              = 3.0;     // [N*m]


    // the maximum amount of torque which can be provided by your gripper
    m_specifications.m_maxGripperForce                = 0.0;     // [N]

    // the maximum closed loop linear stiffness in [N/m] along the x,y,z axis
    m_specifications.m_maxLinearStiffness             = 1500.0; // [N/m]

    // the maximum amount of angular stiffness
    m_specifications.m_maxAngularStiffness            = 30.0;    // [N*m/Rad]

    // the maximum amount of stiffness supported by the gripper
    m_specifications.m_maxGripperLinearStiffness      = 0;   // [N*m]

    // the radius of the physical workspace of the device (x,y,z axis)
    m_specifications.m_workspaceRadius                = 0.45;     // [m]

    // the maximum opening angle of the gripper
    m_specifications.m_gripperMaxAngleRad             = cDegToRad(0.0);


    ////////////////////////////////////////////////////////////////////////////
    /*
        DAMPING PROPERTIES:

        Start with small values as damping terms can be high;y sensitive to
        the quality of your velocity signal and the spatial resolution of your
        device. Try gradually increasing the values by using example "01-devices"
        and by enabling viscosity with key command "2".
    */
    ////////////////////////////////////////////////////////////////////////////

    // Maximum recommended linear damping factor Kv
    m_specifications.m_maxLinearDamping             = 3.0;   // [N/(m/s)]

    //! Maximum recommended angular damping factor Kv (if actuated torques are available)
    m_specifications.m_maxAngularDamping            = 0.25;    // [N*m/(Rad/s)]

    //! Maximum recommended angular damping factor Kv for the force gripper. (if actuated gripper is available)
    m_specifications.m_maxGripperAngularDamping     = 0.0;    // [N*m/(Rad/s)]


    //--------------------------------------------------------------------------
    // CHARACTERISTICS: (The following are of boolean type: (true or false)
    //--------------------------------------------------------------------------

    // does your device provide sensed position (x,y,z axis)?
    m_specifications.m_sensedPosition                = true;

    // does your device provide sensed rotations (i.e stylus)?
    m_specifications.m_sensedRotation                = true;

    // does your device provide a gripper which can be sensed?
    m_specifications.m_sensedGripper                 = false;

    // is you device actuated on the translation degrees of freedom?
    m_specifications.m_actuatedPosition              = true;

    // is your device actuated on the rotation degrees of freedom?
    m_specifications.m_actuatedRotation              = true;

    // is the gripper of your device actuated?
    m_specifications.m_actuatedGripper               = false;

    // can the device be used with the left hand?
    m_specifications.m_leftHand                      = true;

    // can the device be used with the right hand?
    m_specifications.m_rightHand                     = true;


    ////////////////////////////////////////////////////////////////////////////
    /*
        STEP 2:

        Here, you shall  implement code which tells the application if your
        device is actually connected to your computer and can be accessed.
        In practice this may be consist in checking if your I/O board
        is active or if your drivers are available.

        If your device can be accessed, set:
        m_systemAvailable = true;

        Otherwise set:
        m_systemAvailable = false;

        Your actual code may look like:

        bool result = checkIfMyDeviceIsAvailable()
        m_systemAvailable = result;

        If want to support multiple devices, using the method argument
        a_deviceNumber to know which device to setup
    */
    ////////////////////////////////////////////////////////////////////////////

    m_deviceAvailable = false; // this value should become 'true' when the device is available.
	m_ip = "127.0.0.1#53210";
	m_VC = NULL;
	m_scale = 1.0;
	m_toolOffset[0] = m_toolOffset[1] = m_toolOffset[2] = 0.0f;
	m_baseFrame[0] = m_baseFrame[1] = m_baseFrame[2] = m_baseFrame[3] = m_baseFrame[4] = m_baseFrame[5] = 0.0;
	m_baseFrame[6] = 1.0;

    // *** INSERT YOUR CODE HERE ***
	FILE *config = fopen(VIRTUOSE_CFG, "r");
	if (config)
	{
		int devNb = -1;
		char line[1024];
		while(fgets(line, 1024, config))
		{
			if (line[0] == '#')
				continue;
			if (line[0] == '[')
				devNb ++;
			if (devNb != a_deviceNumber)
				continue;
			if (line[0] == '[')
			{
				char *closeBracket = strchr(line, ']');
				closeBracket[0] = '\0';
				m_specifications.m_modelName = &line[1];
				continue;
			}
			char *token;
			token = strtok(line, "=");
			if (token == NULL)
				return;
			if (strcmp(token, "Addr") == 0)
			{
				token = strtok(NULL, "=");
				m_ip = (char *) malloc (strlen(token) + 1);
				strcpy(m_ip, token);
				continue;
			}
			if (strcmp(token, "Power") == 0)
			{
				token = strtok(NULL, "=");
				sscanf(token, "%d", &m_power);
				if (m_power != 1)
					m_power = 0;
				continue;
			}
			if (strcmp(token, "Button1") == 0)
			{
				token = strtok(NULL, "=");
				sscanf(token, "%d", &m_button1);
				continue;
			}
			if (strcmp(token, "Button2") == 0)
			{
				token = strtok(NULL, "=");
				sscanf(token, "%d", &m_button2);
				continue;
			}
			if (strcmp(token, "MaxForce") == 0)
			{
				float value;
				token = strtok(NULL, "=");
				sscanf(token, "%f", &value);
				m_specifications.m_maxLinearForce = value;
				continue;
			}
			if (strcmp(token, "MaxTorque") == 0)
			{
				float value;
				token = strtok(NULL, "=");
				sscanf(token, "%f", &value);
				m_specifications.m_maxAngularTorque = value;
				continue;
			}
			if (strcmp(token, "KTrans") == 0)
			{
				float value;
				token = strtok(NULL, "=");
				sscanf(token, "%f", &value);
				m_specifications.m_maxLinearStiffness = value;
				continue;
			}
			if (strcmp(token, "BTrans") == 0)
			{
				float value;
				token = strtok(NULL, "=");
				sscanf(token, "%f", &value);
				m_specifications.m_maxLinearDamping = value;
				continue;
			}
			if (strcmp(token, "KRot") == 0)
			{
				float value;
				token = strtok(NULL, "=");
				sscanf(token, "%f", &value);
				m_specifications.m_maxAngularStiffness = value;
				continue;
			}
			if (strcmp(token, "BRot") == 0)
			{
				float value;
				token = strtok(NULL, "=");
				sscanf(token, "%f", &value);
				m_specifications.m_maxAngularDamping = value;
				continue;
			}
			if (strcmp(token, "Scaling") == 0)
			{
				float value;
				token = strtok(NULL, "=");
				sscanf(token, "%f", &value);
				m_scale = value;
				continue;
			}
			if (strcmp(token, "ToolOffsetX") == 0)
			{
				float value;
				token = strtok(NULL, "=");
				sscanf(token, "%f", &value);
				m_toolOffset[0] = value;
				continue;
			}
			if (strcmp(token, "ToolOffsetY") == 0)
			{
				float value;
				token = strtok(NULL, "=");
				sscanf(token, "%f", &value);
				m_toolOffset[1] = value;
				continue;
			}
			if (strcmp(token, "ToolOffsetZ") == 0)
			{
				float value;
				token = strtok(NULL, "=");
				sscanf(token, "%f", &value);
				m_toolOffset[2] = value;
				continue;
			}
			if (strcmp(token, "BaseRotZ") == 0)
			{
				float value;
				token = strtok(NULL, "=");
				sscanf(token, "%f", &value);
				m_baseFrame[5] = sin(value / 180.0 * M_PI / 2.0);
				m_baseFrame[6] = cos(value / 180.0 * M_PI / 2.0);
				continue;
			}

		}
		fclose(config);
	}

	if (m_ip != NULL)
	{
		m_VC = virtOpen(m_ip);
        printf("%d\n", m_VC);
		if (m_VC != NULL)
		{
			m_deviceAvailable = true;
			virtClose(m_VC);
			m_VC = NULL;
		}
	}
}


//==============================================================================
/*!
    Destructor of cMyCustomDevice.
*/
//==============================================================================
cVirtuoseDevice::~cVirtuoseDevice()
{
    // close connection to device
    if (m_deviceReady)
    {
        close();
    }

	if (m_ip != NULL)
		free(m_ip);
}


//==============================================================================
/*!
    This method opens a connection to your device.

    \return __true__ if the operation succeeds, __false__ otherwise.
*/
//==============================================================================
bool cVirtuoseDevice::open()
{
    // check if the system is available
    if (!m_deviceAvailable) return (C_ERROR);

    // if system is already opened then return
    if (m_deviceReady) return (C_ERROR);

    ////////////////////////////////////////////////////////////////////////////
    /*
        STEP 3:

        Here you shall implement to open a connection to your
        device. This may include opening a connection to an interface board
        for instance or a USB port.

        If the connection succeeds, set the variable 'result' to true.
        otherwise, set the variable 'result' to false.

        Verify that your device is calibrated. If your device
        needs calibration then call method calibrate() for wich you will
        provide code in STEP 5 further below.
    */
    ////////////////////////////////////////////////////////////////////////////

    bool result = C_ERROR; // this value will need to become "C_SUCCESS" for the device to be marked as ready.

    // *** INSERT YOUR CODE HERE ***
    // result = openConnectionToMyDevice();
	if (m_ip == NULL)
		return (C_ERROR);

	m_VC = virtOpen(m_ip);
	if (m_VC == NULL)
		return (C_ERROR);
	result = C_SUCCESS;

	virtSetCommandType(m_VC, COMMAND_TYPE_IMPEDANCE);
	virtSetPowerOn(m_VC, m_power);
	virtSetSpeedFactor(m_VC, m_scale);
	virtSetIndexingMode(m_VC, INDEXING_ALL_FORCE_FEEDBACK_INHIBITION);
	// Build base frame as float array and apply
	float baseFrame[7];
	for (int i=0; i<7; i++)
		baseFrame[i] = m_baseFrame[i];
	virtSetBaseFrame(m_VC, baseFrame);

    // update device status
    if (result)
    {
        m_deviceReady = true;
        return (C_SUCCESS);
    }
    else
    {
        m_deviceReady = false;
        return (C_ERROR);
    }
}


//==============================================================================
/*!
    This method closes the connection to your device.

    \return __true__ if the operation succeeds, __false__ otherwise.
*/
//==============================================================================
bool cVirtuoseDevice::close()
{
    // check if the system has been opened previously
    if (!m_deviceReady) return (C_ERROR);

    ////////////////////////////////////////////////////////////////////////////
    /*
        STEP 4:

        Here you shall implement code that closes the connection to your
        device.

        If the operation fails, simply set the variable 'result' to C_ERROR   .
        If the connection succeeds, set the variable 'result' to C_SUCCESS.
    */
    ////////////////////////////////////////////////////////////////////////////

    bool result = C_SUCCESS; // if the operation fails, set value to C_ERROR.

    // *** INSERT YOUR CODE HERE ***
    // result = closeConnectionToMyDevice()
	if (m_VC != NULL)
		virtClose(m_VC);
	m_VC = NULL;

    // update status
    m_deviceReady = false;

    return (result);
}


//==============================================================================
/*!
    This method calibrates your device.

    \return __true__ if the operation succeeds, __false__ otherwise.
*/
//==============================================================================
bool cVirtuoseDevice::calibrate(bool a_forceCalibration)
{
    // check if the device is read. See step 3.
    if (!m_deviceReady) return (C_ERROR);

    ////////////////////////////////////////////////////////////////////////////
    /*
        STEP 5:

        Here you shall implement code that handles a calibration procedure of the
        device. In practice this may include initializing the registers of the
        encoder counters for instance.

        If the device is already calibrated and  a_forceCalibration == false,
        the method may immediately return without further action.
        If a_forceCalibration == true, then the calibrartion procedure
        shall be executed even if the device has already been calibrated.

        If the calibration procedure succeeds, the method returns C_SUCCESS,
        otherwise return C_ERROR.
    */
    ////////////////////////////////////////////////////////////////////////////

    bool result = C_SUCCESS;

    // *** INSERT YOUR CODE HERE ***

    // error = calibrateMyDevice()

    return (result);
}


//==============================================================================
/*!
    This method returns the number of devices available from this class of device.

    \return __true__ if the operation succeeds, __false__ otherwise.
*/
//==============================================================================
unsigned int cVirtuoseDevice::getNumDevices()
{
    ////////////////////////////////////////////////////////////////////////////
    /*
        STEP 6:

        Here you shall implement code that returns the number of available
        haptic devices of type "cMyCustomDevice" which are currently connected
        to your computer.

        In practice you will often have either 0 or 1 device. In which case
        the code here below is already implemented for you.

        If you have support more than 1 devices connected at the same time,
        then simply modify the code accordingly so that "numberOfDevices" takes
        the correct value.
    */
    ////////////////////////////////////////////////////////////////////////////

    // *** INSERT YOUR CODE HERE, MODIFY CODE below ACCORDINGLY ***

    int numberOfDevices = 0;  // At least set to 1 if a device is available.

	FILE *config = fopen(VIRTUOSE_CFG, "r");
	if (config)
	{
		char line[1024];
		while(fgets(line, 1024, config))
		{
			if (line[0] == '[')
				numberOfDevices ++;
		}
		fclose(config);
	}

    // numberOfDevices = getNumberOfDevicesConnectedToTheComputer();

    return (numberOfDevices);
}

//==============================================================================
/*!
    This function computes the composition of two displacements.

    \param   iDisp1  First displacement (position and quaternion with scalar last).
    \param   iDisp2  Second displacement (position and quaternion with scalar last).
    \param   oDisp  Output (needs to be declared as double[7]).

    \return __true__ if the operation succeeds, __false__ otherwise.
*/
//==============================================================================
static bool ComposeDisplacements(double *iDisp1, double *iDisp2, double *oDisp)
{
	double x1 = iDisp1[0];
	double y1 = iDisp1[1];
	double z1 = iDisp1[2];
	double r1 = iDisp1[6];
	double qx1 = iDisp1[3];
	double qy1 = iDisp1[4];
	double qz1 = iDisp1[5];
	double x2 = iDisp2[0];
	double y2 = iDisp2[1];
	double z2 = iDisp2[2];
	double r2 = iDisp2[6];
	double qx2 = iDisp2[3];
	double qy2 = iDisp2[4];
	double qz2 = iDisp2[5];
	// Rotation part: multiply quaternions
	oDisp[6] = r1*r2 - qx1*qx2 - qy1*qy2 - qz1*qz2;
	oDisp[3] = r1*qx2 + r2*qx1 + qy1*qz2 - qy2*qz1;
	oDisp[4] = r1*qy2 + r2*qy1 + qx2*qz1 - qx1*qz2;
	oDisp[5] = r1*qz2 + r2*qz1 + qx1*qy2 - qx2*qy1;
	// Translation part: rotate 2 with 1
	double a, b, c, d;
	a = qy1*z2 - qz1*y2 + r1*x2;
	b = qz1*x2 - qx1*z2 + r1*y2;
	c = qx1*y2 - qy1*x2 + r1*z2;
	d = qx1*x2 + qy1*y2 + qz1*z2;
	oDisp[0] = - qz1*b + qy1*c + qx1*d + r1*a + x1;
	oDisp[1] = - qx1*c + qz1*a + qy1*d + r1*b + y1;
	oDisp[2] = - qy1*a + qx1*b + qz1*d + r1*c + z1;
	return true;
}

//==============================================================================
/*!
    This function computes the cross product of two vectors.

    \param   iVect1  First vector (three values).
    \param   iVect2  Second vector (three values).
    \param   oVect  Output (needs to be declared as double[3]).

    \return __true__ if the operation succeeds, __false__ otherwise.
*/
//==============================================================================
static bool ComputeCrossProduct(double *iVect1, double *iVect2, double *oVect)
{
	double x1 = iVect1[0];
	double y1 = iVect1[1];
	double z1 = iVect1[2];
	double x2 = iVect2[0];
	double y2 = iVect2[1];
	double z2 = iVect2[2];
	oVect[0] = y1*z2 - z1*y2;
	oVect[1] = - x1*z2 + z1*x2;
	oVect[2] = x1*y2 - y1*x2;
	return true;
}

//==============================================================================
/*!
    This function applies a quaternion rotation to a vector.

    \param   iQuat  Quaternion (four values with scalar last).
    \param   iVect  Vector to be rotated (three values).
    \param   oVect  Output (needs to be declared as double[3]).

    \return __true__ if the operation succeeds, __false__ otherwise.
*/
//==============================================================================
static bool RotateVector(double *iQuat, double *iVect, double *oVect)
{
	double r = iQuat[3];
	double qx = iQuat[0];
	double qy = iQuat[1];
	double qz = iQuat[2];
	double x = iVect[0];
	double y = iVect[1];
	double z = iVect[2];
	double a, b, c, d;
	a = r*x + qy*z - qz*y;
	b = r*y + qz*x - qx*z;
	c = r*z + qx*y - qy*x;
	d = qx*x + qy*y + qz*z;
	oVect[0] = - qz*b + qy*c + qx*d + r*a;
	oVect[1] = - qx*c + qz*a + qy*d + r*b;
	oVect[2] = - qy*a + qx*b + qz*d + r*c;
	return true;
}

//==============================================================================
/*!
    This function computes the inverse of a displacement.

    \param   iDisp  Input displacement (position and quaternion with scalar last).
    \param   oDisp  Output (needs to be declared as double[7]).

    \return __true__ if the operation succeeds, __false__ otherwise.
*/
//==============================================================================
static bool DisplacementInverse(double *iDisp, double *oDisp)
{
	double x = iDisp[0];
	double y = iDisp[1];
	double z = iDisp[2];
	double r = iDisp[6];
	double qx = iDisp[3];
	double qy = iDisp[4];
	double qz = iDisp[5];
	oDisp[3] = -qx;
	oDisp[4] = -qy;
	oDisp[5] = -qz;
	oDisp[6] = r;
	RotateVector(&oDisp[3], &iDisp[0], &oDisp[0]);
	oDisp[0] *= -1.0;
	oDisp[1] *= -1.0;
	oDisp[2] *= -1.0;
	return true;
}

//==============================================================================
/*!
    This method returns the position of your device. Units are meters [m].

    \param   a_position  Return value.

    \return __true__ if the operation succeeds, __false__ otherwise.
*/
//==============================================================================
bool cVirtuoseDevice::getPosition(cVector3d& a_position)
{
    // check if the device is read. See step 3.
    if (!m_deviceReady) return (C_ERROR);

    ////////////////////////////////////////////////////////////////////////////
    /*
        STEP 7:

        Here you shall implement code that reads the position (X,Y,Z) from
        your haptic device. Read the values from your device and modify
        the local variable (x,y,z) accordingly.
        If the operation fails return an C_ERROR, C_SUCCESS otherwise

        Note:
        For consistency, units must be in meters.
        If your device is located in front of you, the x-axis is pointing
        towards you (the operator). The y-axis points towards your right
        hand side and the z-axis points up towards the sky.
    */
    ////////////////////////////////////////////////////////////////////////////

    bool result = C_SUCCESS;

    // *** INSERT YOUR CODE HERE, MODIFY CODE below ACCORDINGLY ***

	// Read clutched position of device
	float position[7];
	virtGetPosition(m_VC, position);
	for (int i=0; i<7; i++)
		m_clutchedPos[i] = position[i];
	// Apply inverse of base rotation
	double invBaseFrame[7];
	DisplacementInverse(m_baseFrame, invBaseFrame);
	double clutchedPos[7];
	ComposeDisplacements(invBaseFrame, m_clutchedPos, clutchedPos);
	// Read physical position of device
	float physPosition[7];
	virtGetPhysicalPosition(m_VC, physPosition);
	for (int i=0; i<7; i++)
		m_physicalPos[i] = physPosition[i];
	// Compute clutch offset
	double clutch[7];
	double invPhysPos[7];
	DisplacementInverse(m_physicalPos, invPhysPos);
	//ComposeDisplacements(invPhysPos, m_clutchedPos, clutch);
	ComposeDisplacements(invPhysPos, clutchedPos, clutch);
	clutch[0] = clutchedPos[0] - m_physicalPos[0];
	clutch[1] = clutchedPos[1] - m_physicalPos[1];
	clutch[2] = clutchedPos[2] - m_physicalPos[2];
	// Apply tool offset to physical position
	double toolOffset[7] = { m_toolOffset[0], m_toolOffset[1], m_toolOffset[2], 0.0f, 0.0f, 0.0f, 1.0f };
	double physToolPos[7];
	ComposeDisplacements(m_physicalPos, toolOffset, physToolPos);
	// Apply clutch
	ComposeDisplacements(physToolPos, clutch, m_position);
	m_position[0] = physToolPos[0] + clutch[0];
	m_position[1] = physToolPos[1] + clutch[1];
	m_position[2] = physToolPos[2] + clutch[2];
	// Apply base rotation
	ComposeDisplacements(m_baseFrame, m_position, m_position);

    // store new position values
    a_position.set(m_position[0], m_position[1], m_position[2]);

    // estimate linear velocity
    estimateLinearVelocity(a_position);

    // exit
    return (result);
}


//==============================================================================
/*!
    This method returns the orientation frame of your device end-effector

    \param   a_rotation  Return value.

    \return __true__ if the operation succeeds, __false__ otherwise.
*/
//==============================================================================
bool cVirtuoseDevice::getRotation(cMatrix3d& a_rotation)
{
    // check if the device is read. See step 3.
    if (!m_deviceReady) return (C_ERROR);

    ////////////////////////////////////////////////////////////////////////////
    /*
        STEP 8:

        Here you shall implement code which reads the orientation frame from
        your haptic device. The orientation frame is expressed by a 3x3
        rotation matrix. The 1st column of this matrix corresponds to the
        x-axis, the 2nd column to the y-axis and the 3rd column to the z-axis.
        The length of each column vector should be of length 1 and vectors need
        to be orthogonal to each other.

        Note:
        If your device is located in front of you, the x-axis is pointing
        towards you (the operator). The y-axis points towards your right
        hand side and the z-axis points up towards the sky.

        If your device has a stylus, make sure that you set the reference frame
        so that the x-axis corresponds to the axis of the stylus.
    */
    ////////////////////////////////////////////////////////////////////////////

    bool result = C_SUCCESS;

    // variables that describe the rotation matrix
    double r00, r01, r02, r10, r11, r12, r20, r21, r22;
    cMatrix3d frame;
    frame.identity();

    // *** INSERT YOUR CODE HERE, MODIFY CODE below ACCORDINGLY ***

	double a = m_position[6];
    double b = m_position[3];
    double c = m_position[4];
    double d = m_position[5];
    r00 = a * a + b * b - c * c - d * d;
    r01 = 2 * b * c - 2 * a * d;
    r02 = 2 * b * d + 2 * a * c;
    r10 = 2 * b * c + 2 * a * d;
    r11 = a * a - b * b + c * c - d * d;
    r12 = 2 * c * d - 2 * a * b;
    r20 = 2 * b * d - 2 * a * c;
    r21 = 2 * c * d + 2 * a * b;
    r22 = a * a - b * b - c * c + d * d;

    frame.set(r00, r01, r02, r10, r11, r12, r20, r21, r22);

    // store new rotation matrix
    a_rotation = frame;

    // estimate angular velocity
    estimateAngularVelocity(a_rotation);

    // exit
    return (result);
}


//==============================================================================
/*!
    This method returns the gripper angle in radian.

    \param   a_angle  Return value.

    \return __true__ if the operation succeeds, __false__ otherwise.
*/
//==============================================================================
bool cVirtuoseDevice::getGripperAngleRad(double& a_angle)
{
    // check if the device is read. See step 3.
    if (!m_deviceReady) return (C_ERROR);

    ////////////////////////////////////////////////////////////////////////////
    /*
        STEP 9:
        Here you may implement code which reads the position angle of your
        gripper. The result must be returned in radian.

        If the operation fails return an error code such as C_ERROR for instance.
    */
    ////////////////////////////////////////////////////////////////////////////

    bool result = C_SUCCESS;

    // *** INSERT YOUR CODE HERE, MODIFY CODE below ACCORDINGLY ***

    // return gripper angle in radian
    a_angle = 0.0;  // a_angle = getGripperAngleInRadianFromMyDevice();

    // estimate gripper velocity
    estimateGripperVelocity(a_angle);

    // exit
    return (result);
}


//==============================================================================
/*!
    This method sends a force [N] and a torque [N*m] and gripper torque [N*m]
    to your haptic device.

    \param   a_force  Force command.
    \param   a_torque  Torque command.
    \param   a_gripperForce  Gripper force command.

    \return __true__ if the operation succeeds, __false__ otherwise.
*/
//==============================================================================
bool cVirtuoseDevice::setForceAndTorqueAndGripperForce(const cVector3d& a_force,
                                                       const cVector3d& a_torque,
                                                       const double a_gripperForce)
{
    // check if the device is read. See step 3.
    if (!m_deviceReady) return (C_ERROR);

    ////////////////////////////////////////////////////////////////////////////
    /*
        STEP 10:

        Here you may implement code which sends a force (fx,fy,fz),
        torque (tx, ty, tz) and/or gripper force (gf) command to your haptic device.

        If your device does not support one of more of the force, torque and
        gripper force capabilities, you can simply ignore them.

        Note:
        For consistency, units must be in Newtons and Newton-meters
        If your device is placed in front of you, the x-axis is pointing
        towards you (the operator). The y-axis points towards your right
        hand side and the z-axis points up towards the sky.

        For instance: if the force = (1,0,0), the device should move towards
        the operator, if the force = (0,0,1), the device should move upwards.
        A torque (1,0,0) would rotate the handle counter clock-wise around the
        x-axis.
    */
    ////////////////////////////////////////////////////////////////////////////

    bool result = C_SUCCESS;

    // store new force value.
    m_prevForce = a_force;
    m_prevTorque = a_torque;
    m_prevGripperForce = a_gripperForce;

    // retrieve force, torque, and gripper force components in individual variables

	// Read physical position of device
	float position[7];
	virtGetPhysicalPosition(m_VC, position);
	// Project tool offset in world
	double virtDisp[7] = { position[0], position[1], position[2], position[3], position[4], position[5], position[6] };
	// Apply base rotation
	double baseFrame[7];
	for (int i=0; i<7; i++)
		baseFrame[i] = m_baseFrame[i];
	ComposeDisplacements(baseFrame, virtDisp, virtDisp);
	double offset[3];
	RotateVector(&virtDisp[3], m_toolOffset, offset);

	// Compute tool offset
	double force[3] = { a_force(0), a_force(1), a_force(2) };
	ComputeCrossProduct(force, offset, force);

    //double gf = a_gripperForce;

    // *** INSERT YOUR CODE HERE ***
	float virtForce[6];
	virtForce[0] = a_force(0);
	virtForce[1] = a_force(1);
	virtForce[2] = a_force(2);
	virtForce[3] = a_torque(0) - force[0];
	virtForce[4] = a_torque(1) - force[1];
	virtForce[5] = a_torque(2) - force[2];
	virtSetForce(m_VC, virtForce);

    // exit
    return (result);
}


//==============================================================================
/*!
    This method returns status of all user switches
    [__true__ = __ON__ / __false__ = __OFF__].

    \param  a_userSwitches  Return the 32-bit binary mask of the device buttons.

    \return __true__ if the operation succeeds, __false__ otherwise.
*/
//==============================================================================
bool cVirtuoseDevice::getUserSwitches(unsigned int& a_userSwitches)
{
    // check if the device is read. See step 3.
    if (!m_deviceReady) return (C_ERROR);

    ////////////////////////////////////////////////////////////////////////////
    /*
        STEP 11:

        Here you shall implement code that reads the status all user switches
        on your device. For each user switch, set the associated bit on variable
        a_userSwitches. If your device only has one user switch, then set
        a_userSwitches to 1, when the user switch is engaged, and 0 otherwise.
    */
    ////////////////////////////////////////////////////////////////////////////

    // *** INSERT YOUR CODE HERE ***
    a_userSwitches = 0;
	int btn0, btn1;
	virtGetButton(m_VC, m_button1, &btn0);
	virtGetButton(m_VC, m_button2, &btn1);
    a_userSwitches = btn0 + (btn1 << 1);

    return (C_SUCCESS);
}


//------------------------------------------------------------------------------
}       // namespace chai3d
//------------------------------------------------------------------------------
#endif  // C_ENABLE_CUSTOM_DEVICE_SUPPORT
//------------------------------------------------------------------------------
