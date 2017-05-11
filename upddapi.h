#ifndef TBAPI_H 
#define TBAPI_H 

#include "stdint.h"

#undef TBAPIDLLPFX 
#ifndef TBCHAR 
#ifdef UNDER_CE
#define TBCHAR wchar_t
#define _TBT(x)      L ## x
#else
#define TBCHAR char
#define _TBT 
#endif
#endif

#ifndef t_string 
  #ifdef UNDER_CE
    #define t_string wstring
  #else
    #define t_string string
  #endif
#endif

 
#ifdef INTERNALAPI
// this mode is for internal touch-base use 
#ifdef _WIN32
//#pragma comment(linker,"/nodefaultlib:libcmt") 
//#pragma comment(linker,"/nodefaultlib:libcmtd") 
#pragma comment(linker,"/nodefaultlib:msvcrtd") 
#pragma comment(linker,"/nodefaultlib:msvcrt") 
#pragma comment(lib,"setupapi.lib")
#endif
#endif 

#ifdef _WIN32
#define TBCALL _stdcall
#else
#define TBCALL
#endif

#ifdef TBAPIDLL_EXPORTS 
#ifdef _WIN32
#define TBAPI __declspec(dllexport) TBCALL
#pragma warning(disable:4518)            
#endif
#else
#ifdef _WIN32
#define TBAPI __declspec(dllimport) TBCALL
#pragma warning(disable:4518)            
#endif
//#define TBAPIDLLPFX 
#endif 

#ifndef TBAPI 
#define TBAPI 
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#define TBBOOL uint16_t


// constants that define data types to be read
// can be ORed, eg _EventTypeXY | _EventTypeEvent
// to read pointer co-ordinates and button events

#define _EventTypeXY 0x0001    // pointer co-ordinates
#define _EventTypeEval 0x0002    // change in evaluation state 
#define _EventTypeData 0x0008  // non-pointer data
#define _EventTypeToolbar 0x0010  // toolbar events 
#define _EventConfiguration 0x0020  // OBSOLESCENT - typo for _EventTypeConfiguration, this value will be removed in due course  
#define _EventTypeConfiguration 0x0020  // notifications of changes to driver configuration and state 
#define _EventTypeZ 0x0080   // notifications of change in Z values
#define _EventTypeRelative 0x0100 // notifications of relative movement
#define _EventTypeUnload 0x0200  // the driver is about to attempt an unload 
#define _EventTypeXYNoMask 0x1000    // same as _EventTypeXY but not masked by toolbars and surrounds used for calibration  -- WAS _EventTypeXYCal prior to V6
#define _EventTypeInteractiveTouch 0x4000 // mouse pointer state events for interactive touch mode
#ifdef UPDD_GESTURES
#define _EventTypeGesture 0x8000  
#endif
#define _EventTypePlayUPDDSound 0x800000  // play a sound defined for this device
#define _EventTypeMouseEvent 0x1000000  // raw data sent to mouse port 
#define _EventTypeSecBlock 0x2000000  // touch data was received when a security block was in place 
#define _EventTypeRawMouse 0x8000000  // internal use only 
#define _EventTypeToolbarEx 0x10000000  // extended toolbar events 
#define _EventTypeLogicalEvent 0x20000000  // state changes passed to operating system 
#define _EventTypePhysicalEvent 0x40000000  // changes in the actual "touching" state OBSOLESCENT
#define _EventTypeDigitiserEvent 0x4000000  

#define _EventTypeFlagsEvent 0x80000000  // EVNN / L / R bits OBSOLESCENT

#define CONFIG_EVENT_SETTINGS 1             // one or more updd settings have been changed 
#define CONFIG_EVENT_CONCURRENCY_SIGNAL  2  // a signal was requested by a call to TBApiRegisterProgram 
#define CONFIG_EVENT_CONNECT 3              // a client connection to the driver has been established 
#define CONFIG_EVENT_DISCONNECT 4           // the client connection to the driver has disconnected 
#define CONFIG_EVENT_UNLOAD 5               // the driver has requested termination of this application, typically for uninstallation
#define CONFIG_EVENT_DEVICE 6               // notificatiopn of a change in device state
#define CONFIG_EVENT_AUTOCONFIGURE 7        // an auto configure operation has been triggered 
#define CONFIG_EVENT_CONCURRENCY_CHANGE  8  // a program was registered with TBApiRegisterProgram or deregistered 
#define CONFIG_EVENT_MONITOR_DETECT  9      // sent at beginning and end of a monitor detection sequence 

#define TOUCH_BIT_FLAGS_LEFT 0x1
#define TOUCH_BIT_FLAGS_RIGHT 0x2 
#define TOUCH_BIT_FLAGS_IN_RANGE 0x8

#define PEN_BIT_FLAGS_TIP 0x1
#define PEN_BIT_FLAGS_BARREL 0x2 
#define PEN_BIT_FLAGS_ERASER 0x4
#define PEN_BIT_FLAGS_IN_RANGE 0x8
#define PEN_BIT_FLAGS_INVERT 0x10

#define DIGITIZER_TYPE_PEN 0x2
#define DIGITIZER_TYPE_TOUCH 0x4

#define HTBDEVICE unsigned char
#define TBSTYLUS unsigned char
#define TB_INVALID_HANDLE_VALUE 0x00

#define MAXSTYLENAME 32
#define MAXCALPOINTS 25

#ifndef TBAPIDEFNS_H 
#define TBAPIDEFNS_H 
 
#pragma pack (push, 1)
struct _PointerEvent // was PointerData prior to V6
{
  HTBDEVICE hDevice;    // device handle that this event relates to or 0 if it is not device specific 
  TBSTYLUS hStylus;     // stylus number, (also know as contact number or touch number) was HTBSTYLUS prior to V6 
  uint64_t type;   // data type of the event, indicates which of the items in the union below is populated
  uint64_t length; // length of data (currently only non-pointer data) 
  unsigned char touchDelegated; // set to true (1) if this app should act as the primary provider of touch. 
  unsigned char usbConfiguration;
  unsigned char usbInterface;
  unsigned char hidEndpoint;
  unsigned char hidReportid;
  unsigned char calibrating;  // set to true (1) if calibration is active; most client apps (ie non calibration apps) should ignore events with this set 
  uint8_t monitor_number;
  uint8_t reserved_byte[3];
  uint32_t reserved[15];
//  unsigned long reserved[16];

//  unsigned long tick;		// indicates relative time data was read 
  union _pe 		          // only one of the following is used, as indicated by the "type" member
  {
    struct  _digitiserEvent
    {
      union  _de
      {
        struct _penEvent
        {
          uint8_t tipSwitch : 1;        // bit flags relating to pen devices, relates to PEN_BIT_FLAGS_XXX above
          uint8_t barrelSwitch : 1;
          uint8_t invert : 1;
          uint8_t inrange : 1;
          uint8_t reserved1 : 1;
          uint8_t reserved2 : 1;
          uint8_t reserved3 : 1;
          uint8_t reserved4 : 1;
          uint32_t reserved5;
        }penEvent;
        struct _touchEvent
        {
          uint8_t touchingLeft : 1; // bit flags relating to regular touch devices, relates to TOUCH_BIT_FLAGS_XXX above
          uint8_t touchingRight : 1;
          uint8_t unused : 1;
          uint8_t inrange : 1;
        }touchEvent;
      }de;
      uint8_t deltaBits;          // a bit mask to indicate which bits are changed since last _digitiserEvent 
      uint8_t validBits;          // a bit mask to indicate which bits are supported by the originating hardware 
      int32_t screenx;            // screen co-ordinate values, these values are in screen pixels and take account of the co-ordinate range of the associated monitors
      int32_t screeny;	          // so for example with 2 monitors, resolution 1024 x 768 side by side; with the left monitor being the primary, 
                                  // touching the centre of the right gives about 1536, 384
      int32_t internalx;          // the corresponding windows co-ordinate value, the primary monitor has the range 0xffff, and other monitors are scaled from that
      int32_t internaly;	        // so in the example given above the result is 0x17fee, 0x7fff
      int32_t calx;               // the calibrated co-ordinates values; a value from 0 - 0xffff, giving the absolute position of touch in the range of the originating hardware 
      int32_t caly;	              // so for example touching the centre of a screen will give  around 7ff regardless of the associated monitor
      TBBOOL zSupport;            // set to TRUE (1) if the originating hardware supports z values
      uint32_t z;                 // the raw z value reported by the controller, typically this is used to indicate pressure

      TBBOOL isTimed;             // set to TRUE (1) if the event is triggered by a timeout (eg liftoff time)  
      TBBOOL isToolbar;           // set to TRUE (1) if the event is for a touch start started in a toolbar
      TBBOOL stylusSupport;       // set to TRUE (1) if the originating hardware supports stylus values
      uint8_t digitizerType;      // see DIGITIZER_TYPE_xxx
    }digitiserEvent;

    struct _xy		// co-ordinate data
    {	
      int32_t rawx;	// the raw X value from the controller (normalised) 
      int32_t rawy;	// the raw Y value from the controller (normalised) 
      int32_t calx; // the corresponding calibrated value 
      int32_t caly;	//           --- ditto ---
      int32_t screenx; // the corresponding screen co-ordinate value 
      int32_t screeny;	//           --- ditto ---
      int32_t internalx; // the corresponding windows co-ordinate value 
      int32_t internaly;	//           --- ditto ---
    }xy;				
    struct _z		// pressure data
    {	
      uint32_t rawz;	// the raw z value from the controller
    }z;				
    struct _logicalEvent		
    {
      TBBOOL left;            // does this represent a left mouse button action 	
      TBBOOL state;  	        // the value that the state changed to  	
      TBBOOL timed;           // whether the change is triggered by a timeout (eg liftoff time)  
      TBBOOL wasToolbarTouch; // was the touch in a toolbar area when the state change to true
    }logicalEvent;                             
    struct  _physicalEvent  
    {
      TBBOOL state;           // the value that the state changed to  	
      TBBOOL timed;           // whether the change is triggered by a timeout (eg liftoff time)  
      TBBOOL wasToolbarTouch; // was the touch in a toolbar area when the state change to true
    }physicalEvent;                             

    // following structure is obsolescent; you should use _touchEvent instead 
    // but currently still supported in the api so you can uncomment this if needed; however we recommend converting to _touchEvent  
    //struct _flagsEvent
    //{
    //  TBBOOL hasEVNN;         // true if the evnn field has valid data
    //  uint32_t evnn;          // value of the EVNN bits in the data packet
    //  TBBOOL hasLBit;         // true if the lBit field has valid data
    //  TBBOOL lBit;            // value of the L bit in the data packet
    //  TBBOOL hasRBit;         // true if the rBit field has valid data
    //  TBBOOL rBit;            // value of the R bit in the data packet
    //}flagsEvent;                             
    struct _data  // non-pointer data
    {
      uint8_t byte[64];
    }data;
    struct _toolbar  // a toolbar area was touched 
    {
      uint16_t toolbarid; // the identifier of the toolbar 
      uint16_t row;  // row # of button
      uint16_t col;  // column number of button
      uint8_t press;          // 0 = release, 1 = press only set in >UPDD5.1.0  and _EventTypeToolbarEx requested, otherwise only presses delivered 
      uint8_t right;          // 1 if the right click bit was set when on the toolbar  
    }toolbar;

    struct _interactiveTouch  // co-ordinate data 
    {  
      uint32_t ticks;   // ticks since touch  
      uint32_t maxTicks; // tick count at which icon change will occur  
    }interactiveTouch ;     
    struct _sound
    {
      uint32_t fileIndex;
      uint32_t reserved1;
      uint32_t reserved2;
    }sound;
    struct _eval
    {
      uint16_t clicksRemaining;
      uint32_t packageExpired;
    }eval;


    //struct _mouseEvent mouseEvent;
    //struct _rawMouseEvent rawMouseEvent;
#ifdef UPDD_GESTURES
#include "gestures_event.inl" 
#endif

    struct _config
    {
      uint32_t configEventType;
      union _ce
      {
        uint8_t configText[64 - sizeof(uint32_t)];
        uint32_t signalValue; 
      }ce;
    }config;


  }pe; 
};
#pragma pack (pop)

#endif // TBAPIDEFNS_H

#ifndef DRIVERSTRINGS	

//function pointer definition for data callback 
typedef void (TBCALL *TB_EVENT_CALL)(unsigned long context, struct _PointerEvent* aEvent);
	  
// V6 Migration note TBApiInit, TBApiInitEx, TBApiTerminate and TBApiDefaultSettingsPath are no longer used 

//void TBAPI TBApiOpen(int argc, char* argv[]);
void TBAPI TBApiOpen();
// Establishes a connection to the device driver
// most API functions require an open connection
// NB only call this once in your program, typically at startup

// note that the connection to the driver is performed asynchronously
// use TBApiIsDriverConnected to check the status of the connection



void TBAPI TBApiClose();
// Closes the connection to the device driver
// NB only call this once in your program, typically at termination


//TBBOOL TBAPI TBApiIsApiActive();
TBBOOL TBAPI TBApiIsDriverConnected();
// Returns a TBBOOL indication of whether a valid driver interface exists
// ie has TBApiOpen been sucessfully called. 

TBBOOL TBAPI TBApiGetDriverVersion(TBCHAR* aVersion);
// returns -- 0 = fail, 1 = OK
// aVersion must point to an address to receive the version number of the driver, 16 bytes should be allocated 

void TBAPI TBApiGetApiVersion(TBCHAR* aVersion);
// aVersion must point to an address to receive the version number of the API, 16 bytes should be allocated 


// --------------------------------------------------------------- //
//         a number of functions require a device id               // 
//   the following family of functions provide valid device id's   // 
// --------------------------------------------------------------- // 
// 
 
HTBDEVICE TBAPI   TBApiGetRelativeDevice(int o);
// this api simply gets the device by it's order in the internal device list 
// typically used to get the only device in a single device system eg 
//
//  HTBDEVICE device = TBApiGetRelativeDevice(0);
//
//  or to enumerate all devices eg
//
//  HTBDEVICE device = TBApiGetRelativeDevice(0);
//  for(int i=0; device != TB_INVALID_HANDLE_VALUE;) 
//  {
//      DoSomethingWithDevice(device);
//      device = TBApiGetRelativeDevice(++i);
//  }
//
//  a return value of TB_INVALID_HANDLE_VALUE means that the requested device does not exist

int TBAPI TBApiGetRelativeDeviceFromHandle( HTBDEVICE aDeviceHandle); 
// this api performs the opposite role to TBApiGetRelativeDevice
// Given a Device handle the (zero based) position in the list is returned 
// a return value of -1 means that the requested device does not exist


TBBOOL TBAPI TBApiGetRotate(HTBDEVICE aDeviceHandle, int32_t* aRotate);
// returns (in aRotate) the rotation factor associated with the device
// returns -- 0 = fail, 1 = OK
//


TBBOOL TBAPI TBApiMousePortInterfaceEnable(HTBDEVICE UNUSED, TBBOOL aState);
// Enable / disables the mouse port interface 
// if the mouse port interface is disabled, the driver functions 
// normally, except that the mouse pointer is not moved and 
// mouse button clicks are not emulated. Data can be read via the api
// returns -- 0 = fail, 1 = OK

TBBOOL TBAPI TBApiRegisterEvent(HTBDEVICE aDeviceHandle, unsigned long aContext, unsigned long aTypes, TB_EVENT_CALL aFunc);
// Informs the API that a function is to be used a callback function for 
// the specified type(s) of data 
// The context value is passed unchanged to the callback function for identification purposes 
// All functions registered MUST be unregistered with TBApiUnregisterEvent
// before the program terminates
//
// In the following example CBFunc is called whenever pointer co-ordinates are processed
// by relative device 1 until TBApiUnregisterEvent is called
//
// **USAGE NOTE**
// the callback function is executed in the context of a dedicated thread 
// therefore only thread safe (reentrant) functions should be called from the callback function
// many windowing api functions are non-reentrant
// if you need to call non-reentrant functions you need to provide synchronisation management 
// a common way to achieve this is to post a message to the primary process thread and perform 
// all non-reentrant operations from the primary thread
//
// example 1; register callback for the first device found
// 
// HTBDEVICE hd = TBApiGetRelativeDevice(0);
//
// TBApiRegisterEvent(hd,0,_EventTypeXY,CBFunc); 
//
// .....
//
// void TBAPI CBFunc(unsigned long context, _PointerData* data)
// {
//      printf("device %d generated x=%d y=%d\n",(int)data->device,(int)data->xy->rawx,(int)data->xy->rawy);
// }
//
// To get data for all devices 0
// 
// example 2; register callback for all devices
// 
// TBApiRegisterEvent(0,_EventTypeXY,CBFunc); 
// 
// returns -- 0 = fail, 1 = OK

TBBOOL TBAPI TBApiRegisterCallbackAsTouchDelegate(TB_EVENT_CALL aFunc);
// Indicate that the specified callback function is to receive delegated touch events only // 
// returns -- 0 = fail, 1 = OK

TBBOOL TBAPI TBApiUnregisterEvent(TB_EVENT_CALL aFunc);
// Removes the specified function from the list of registered callbacks. 
// 
// eg to unregister the callback specified in the above exaple 
// 
// TBApiRegisterEvent(CBFunc); 
//
// returns -- 0 = fail, 1 = OK

// a set of functions to set / retrieve UPDD settings
TBBOOL TBAPI TBApiGetSetting(HTBDEVICE aHandle,const TBCHAR* aName, TBCHAR* aSZ, int lBuff);

TBBOOL TBAPI TBApiGetSettingAsInt(HTBDEVICE aHandle,const TBCHAR* aName, int32_t* val);

TBBOOL TBAPI TBApiSetSetting(HTBDEVICE aHandle, const TBCHAR* aName, const TBCHAR* aSZ, TBBOOL aDeviceSpecific);

TBBOOL TBAPI TBApiSetSettingFromInt(HTBDEVICE aHandle, const TBCHAR* aName, int32_t val, TBBOOL aDeviceSpecific);

// this variant retrieves the default setting for the controller type as opposed to a specific installed instance of the controller
TBBOOL TBAPI TBApiGetControllerSetting(int aControllerHandle, const TBCHAR* aName, TBCHAR* aSZ, int lBuff);


// add a new controller to the device list

// aControllerID               :  the handle of the controller definition as, for example returned by upddutils controllers 
// aDeviceName                 :  a name to identify the entry, pass NULL to assign an auto generated id
// aNewHandle                  :  address to receive he new device handle, can be NULL

TBBOOL TBAPI TBApiAddDevice(int aControllerID, const TBCHAR* aDeviceName, HTBDEVICE* aNewHandle);

TBBOOL TBAPI TBApiDeleteDevice(HTBDEVICE aDevice);

void TBAPI TBApiEnableApiTrace(TBBOOL aEnable);

 
TBBOOL TBAPI TBApiPostPacketBytes(HTBDEVICE aDevice, const char* aData);

TBBOOL TBAPI TBApiInjectTouch(HTBDEVICE aDevice, int x, int y, int st, TBBOOL touching);


////////////////////////////////////////////////////////////////////////////////////////////////////
/// get the position of the primary monitor 
/// and the position and size of a monitor associated with a specified updd device 
////////////////////////////////////////////////////////////////////////////////////////////////////
void TBAPI TBApiGetMonitorMetrics(unsigned aDevice,
                                  long* aPrimaryMonitorWidth, long* aPrimaryMonitorHeight,
                                  long* aMonitorWidth, long* aMonitorHeight, long* aMonitorLeft, long* aMonitorTop);

////////////////////////////////////////////////////////////////////////////////////////////////////
/// get the  position and size of a monitor 
////////////////////////////////////////////////////////////////////////////////////////////////////

TBBOOL TBAPI TBApiGetMonitorMetricsForMonitor(unsigned aMonitor,
                                            long* aMonitorWidth, long* aMonitorHeight, long* aMonitorLeft, long* aMonitorTop);

////////////////////////////////////////////////////////////////////////////////////////////////////
/// get extended info about an api error, currently only applicable to  TBApiReadEEPROM / TBApiWriteEEPROM

////////////////////////////////////////////////////////////////////////////////////////////////////

void TBAPI TBApiGetLastError(TBCHAR* aMsg, int aMaxLength);

////////////////////////////////////////////////////////////////////////////////////////////////////
/// record a program as running or check if already running 
//  mainly used to limit client applications to a single instance
//
// arguments:- 
//           aProgramName:       a name to uniquley identify the program
//           aRegisterAsRunning: if true then record the current process along with the specified name in the list of running programs
//           aFailIfRunning:     if true and another instance of the named program is running the api call is failed
//                               the api call returns false and the process is not recorded in the running list
//           aSignalRunningApps: it true, an event of type _EventConfiguration is sent to existing instancesof the named program
//                               the field configEventType is set to CONFIG_EVENT_CONCURRENCY_SIGNAL
//
//  Example:
//          int main(...) 
//          {
//            ...
//            if (!TBApiRegisterProgram("UPDD Daemon", true, true, false))
//            {
//              TBApiClose();
//              return(1);
//            }
//
////////////////////////////////////////////////////////////////////////////////////////////////////

TBBOOL TBAPI TBApiRegisterProgram(const TBCHAR* aProgramName,TBBOOL aRegisterAsRunning, TBBOOL aFailIfRunning, TBBOOL aSignalRunningApps);


////////////////////////////////////////////////////////////////////////////////////////////////////
TBBOOL TBAPI TBApiLicence(const TBCHAR* aLicenceKey);


////////////////////////////////////////////////////////////////////////////////////////////////////
/// pass  PointerEvent callback to registered client apps  
////////////////////////////////////////////////////////////////////////////////////////////////////

TBBOOL TBAPI TBApiPostPointerEvent(struct _PointerEvent* aEvent);


////////////////////////////////////////////////////////////////////////////////////////////////////
/// is device connected TRUE / FALSE
/// for RS232 devices this refers to whether the driver has an open connection to the com port 
////////////////////////////////////////////////////////////////////////////////////////////////////
TBBOOL TBAPI TBApiIsDeviceConnected(HTBDEVICE aDeviceHandle, TBBOOL* aConnected);


////////////////////////////////////////////////////////////////////////////////////////////////////
/// get the maximum theoretical Z value
////////////////////////////////////////////////////////////////////////////////////////////////////
TBBOOL TBAPI TBApiGetMaxZ(HTBDEVICE aHandle, int32_t* aMaxZ);


////////////////////////////////////////////////////////////////////////////////////////////////////
/// issue an HID set feature request to the device
////////////////////////////////////////////////////////////////////////////////////////////////////


TBBOOL TBAPI TBApiHidSetFeature(
  HTBDEVICE aHandle,
  int aInterface,
  const void*  aReportBuffer,
  uint32_t  aReportBufferLength
  );


////////////////////////////////////////////////////////////////////////////////////////////////////
/// issue an HID get feature request to the device
////////////////////////////////////////////////////////////////////////////////////////////////////


TBBOOL TBAPI TBApiHidGetFeature(
  HTBDEVICE aDevice,
  int aInterface,
  void*  aReportBuffer,
  uint32_t  aReportBufferLength
  );


#endif // DRIVERSTRINGS

#ifdef __cplusplus
} // extern "C"
#endif

#ifdef TBAPIDLL_EXPORTS 
#ifdef _WIN32
#pragma warning(default:4518)
#endif
#endif



#endif







