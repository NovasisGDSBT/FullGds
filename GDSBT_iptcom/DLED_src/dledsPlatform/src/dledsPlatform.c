/************************************************************************/
/*  (C) COPYRIGHT 2015 by Bombardier Transportation                     */
/*                                                                      */
/*  Bombardier Transportation Switzerland Dep. PPC/EUT                  */
/************************************************************************/
/*                                                                      */
/*  PROJECT:      Remote download                                       */
/*                                                                      */
/*  MODULE:       dledsPlatform                                         */
/*                                                                      */
/*  ABSTRACT:     This file contains the platform specific parts        */
/*                for dleds.                                            */
/*                                                                      */
/*                                                                      */
/*                                                                      */
/*  REMARKS:                                                            */
/*                                                                      */
/*  DEPENDENCIES: See include list                                      */
/*                                                                      */
/*  ACCEPTED:                                                           */
/*                                                                      */
/************************************************************************/
/*                                                                      */
/*  HISTORY:                                                            */
/*                                                                      */
/*  version  yy-mm-dd  name       depart.    ref   status               */
/*  -------  --------  ---------  -------    ----  ---------            */
/*    1.0    15-03-27  S.Eriksson GSC/NETPS   --   created              */
/*    1.0    17-04-12  G.Genna    GDS/Novasis --   custom              */
/*                                                                      */
/************************************************************************/


/*******************************************************************************
 * INCLUDES
 */
#include <stdio.h>
#include <stdlib.h>

#include "dleds.h"
#include "dludatarep.h"
#include "dledsVersion.h"
#include "dledsInstall.h"
#include "dleds_icd.h"
#include "dledsDbg.h"
#include "dledsCrc32.h"
#include "dledsPlatform.h"


/*******************************************************************************
 * DEFINES
 */
/* States during initialization */
#define STEP1   1       /* Initiate IPTCom in multi-process environment */
#define STEP2   2       /* Check that IPTCom and TDC are initiated */
#define STEP3   3       /* Create a MD queue to be able to receive the download request messages */
#define STEP4   4       /* Create a MD queue to be able to receive the result messages on the response and status messages */
#define STEP4_1 41      /* Create a MD queue to be able to receive the response of the echo messages */
#define STEP4_2 42      /* Create a MD queue to be able to receive the response of the progress messages */
#define STEP5   5       /* Add IPTcom configuration data neccessary for DLEDS */
#define STEP6   6       /* Add a listener for message data with the given destination URI */
#define STEP7   7       /* Register publisher of PD status message (ComId 271) */
#define STEP8   8       /* All steps of initialization executed successfully */



/*******************************************************************************
 * TYPEDEFS
 */

//#define STATEMACHINE
//#define PRINTCONSOLE
/*******************************************************************************
 * LOCAL FUNCTION DECLARATIONS
 */

/*******************************************************************************
 * LOCAL VARIABLES
 */



/*******************************************************************************
 * GLOBAL VARIABLES
 */
extern char DLEDS_TEMP_STORAGE_FILE_NAME[128];
extern UINT8 localRequest;
extern UINT8 localRequestInProgress;
extern INT32 localRequestResultCode;
extern STATE DLEDS_STATE;
extern TYPE_DLEDS_SESSION_DATA sessionData;
extern INT32 appResultCode;
extern char dledsTempDirectory[256];
extern int interactive;
extern MD_QUEUE requestQueueId;
extern MD_QUEUE resultQueueId;
extern MD_QUEUE echoQueueId;
extern MD_QUEUE progressQueueId;
extern PD_HANDLE pdHandle;
extern char currentDir[256];
extern int dledsDebugOn;

extern const char *statemachine[15];
/*******************************************************************************
 * LOCAL FUNCTION DEFINITIONS
 */

/*******************************************************************************
 *
 * Function name:   dledsPlatformGetOperationMode
 *
 * Abstract:        This function evaluates the operation mode in which the
 *                  operating system is currently running.
 *
 * Return value:    DLEDS_UNKNOWN_MODE:
 *                      Unknown operation mode.
 *
 *                  DLEDS_OSIDLE_MODE:
 *                      The operating system currently runs in idle mode,
 *                      i.e. there are no applications started.
 *
 *                  DLEDS_OSRUN_MODE:
 *                      The operating system currently runs in full
 *                      operational mode, i.e. all applications configured
 *                      for automatic startup are actually started.
 *
 *                  DLEDS_OSDOWNLOAD_MODE:
 *                      The operating system currently runs in a dedicated
 *                      (application-) download mode. Some mandatory system
 *                      software packages have been started, but application
 *                      software startup is intentionally blocked.
 *
 * Globals:         None
 */
 /* FILIPPO */
int operationMode = DLEDS_OSRUN_MODE;

int dledsPlatformGetOperationMode(void)
{
    //int operationMode = DLEDS_UNKNOWN_MODE;

    /******************************************************************************
    *   ADD CODE FOR PLATFORM (START)
    ******************************************************************************/
   //DLEDS_OSIDLE_MODE
   //DLEDS_OSRUN_MODE
   //DLEDS_OSDOWNLOAD_MODE
    /*****************************************************************************
    *   ADD CODE FOR PLATFORM (END)
    ******************************************************************************/

    return operationMode;
}

/*******************************************************************************
 *
 * Function name:   dledsPlatformGetDLAllowed
 *
 * Abstract:        This function retrieves the actual state of the
 *                  download allowed status.
 *
 * Return value:    FALSE:  Download is not allowed
 *                  TRUE:   Download is allowed
 *
 * Globals:         None
 */
int dledsPlatformGetDlAllowed(void)
{
    int result = FALSE;

    /******************************************************************************
    *   ADD CODE FOR PLATFORM (START)
    ******************************************************************************/

    #ifdef PRINTCONSOLE
    printf("Function %s \n",__FUNCTION__);
    #endif

    result= getDlAllowed();

    /*****************************************************************************
    *   ADD CODE FOR PLATFORM (END)
    ******************************************************************************/

    return result;
}

/*******************************************************************************
 *
 * Function name:   dledsPlatformForcedReboot
 *
 * Abstract:        This function shall perform a reboot of the device to
 *                  the defined start up mode mode.
 *
 * Return value:    On success there is no return from this routine as the target
 *                  device will perfom the reset within this function.
 *                  DLEDS_ERROR: Failed to reset device
 *
 * Globals:         None
 */
int dledsPlatformForcedReboot(    int     rebootReason)
                                    /* IN: Wanted start up mode
                                            DLEDS_OSRUN_MODE:
                                                On restart of the device, operating system
                                                shall be started in full operational mode.

                                            DLEDS_OSDOWNLOAD_MODE:
                                                On restart of the device, operating system
                                                shall be started in download mode, where
                                                some software required for application software
                                                download services is started.
                                    */
{
    int result = DLEDS_OK;
    operationMode  = rebootReason;
    #if 0

    /******************************************************************************
    *   ADD CODE FOR PLATFORM (START)
    ******************************************************************************/
    #ifdef PRINTCONSOLE
    printf("Function %s \n",__FUNCTION__);
    #endif

    (void)rebootReason; /* To avoid compiler warning */

    int dledsResult;

    if (rebootReason == DLEDS_OSRUN_MODE)
    {
        dledsResult = forced_reboot(DLEDS_OSRUN_MODE);
    }
    else if (rebootReason == DLEDS_OSDOWNLOAD_MODE)
    {
        dledsResult = forced_reboot(DLEDS_OSDOWNLOAD_MODE);
    }
    else if (rebootReason == DLEDS_OSRESCUE_MODE)
    {
        dledsResult = forced_reboot(DLEDS_OSRESCUE_MODE);
    }
    else
    {
        DebugError1("Illegal reboot reason (%d)", rebootReason);
        dledsResult = TRUE;
    }

    if (dledsResult == TRUE)
    {
        DebugError1("Forced reset (%d) failed", rebootReason);
        result = DLEDS_ERROR;
    }
    else
    {
        result = DLEDS_OK;
    }
    /*****************************************************************************
    *   ADD CODE FOR PLATFORM (END)
    ******************************************************************************/
    #endif
    return result;
}


/*******************************************************************************
 *
 * Function name:   dledsPlatformGetAppMode
 *
 * Abstract:        This function shall get the global variables
 *                  EnddeviceMode and ConfigStatus from the system.
 *
 * Return value:    DLEDS_OK: operation successfull
 *                  DLEDS_ERROR: operation encountered an error
 *
 * Globals:         None
 */
int dledsPlatformGetAppMode(
    int*    IEdMode,            /* IN/OUT: Value for the enddevice mode
                                    0 : Running with application
                                    1 : Application is suspended
                                    2 : Application less mode
                                    3..255: invalid */
    int*    IConfigStatus)      /* IN/OUT: Value for the configuration status
                                    0 : OK
                                    1 : No config. file available
                                    2 : Configuration error
                                    3 : exception in component
                                    4..255: invalid */
{
    int result = DLEDS_ERROR;
    int dledsResult;

    if ((IEdMode == NULL) || (IConfigStatus == NULL))
    {
        return DLEDS_ERROR;
    }

    *IEdMode = 255;
    *IConfigStatus = 255;

    /******************************************************************************
    *   ADD CODE FOR PLATFORM (START)
    ******************************************************************************/
    #ifdef PRINTCONSOLE
    printf("Function %s \n",__FUNCTION__);
    #endif
    //dledsResult = getAppMode(IEdMode, IConfigStatus);
    if (dledsResult == 0)
    {
        result = DLEDS_OK;
    }
    /******************************************************************************
    *   ADD CODE FOR PLATFORM (END)
    ******************************************************************************/

    return result;
}


/*******************************************************************************
 *
 * Function name: dledsPlatformFtp
 *
 * Abstract:      This function retrieves the file from the FTP server.
 *
 * Return value:  DLEDS_OK          - The file is retrieved successfully
 *                DLEDS_ERROR       - Failed to retrieve file
 *
 * Globals:       None
 */
DLEDS_RESULT dledsPlatformFtp(
    char* destFile,             /* IN: Path to directory where file should be stored */
    char* ipAddrStr,            /* IN: IP address of FTP Server */
    T_IPT_IP_ADDR ipAddr,       /* IN: IP address of FTP Server */
    char* fileServerPath,       /* IN: File path on FTP Server */
    char* fileName)             /* IN: File name on FTP Server */
{
    DLEDS_RESULT    result = DLEDS_ERROR;

    if ((destFile == NULL) || (ipAddrStr == NULL) || (fileServerPath == NULL) || (fileName == NULL))
    {
        return DLEDS_ERROR;
    }

    /******************************************************************************
    *   ADD CODE FOR PLATFORM (START)
    ******************************************************************************/
    #ifdef PRINTCONSOLE
    printf("Function %s \n",__FUNCTION__);
    #endif

    (void)ipAddr; /* To avoid compiler warning */
     struct stat     fileStat;

      char    ftpCommand[512];
    int     sysResult;
    char*   pch;

    /* Check if FTP directory exists */
    if (stat(destFile, &fileStat) != 0) {
        if (mkdir(destFile,S_IRWXU | S_IRWXG | S_IRWXO) != 0)
        {
            /* Check if other error than directory already exists */
            /* OK if directory alredy exists, stop for other errors */
            if (errno != EEXIST)
            {
                DebugError1("Failed to create local FTP directory (%s)", destFile);
                DebugError1("errno = %d",errno);
                return DLEDS_ERROR;
            }
        }
    }
    /* Add filename */
    strcat(destFile, "/");
    strcat(destFile, sessionData.requestInfo.fileName);
    /* Prepare FTP command */
    //strcpy(ftpCommand, "ftpget -u anonymous -p anonymous ");
    strcpy(ftpCommand, "ftpget -u Anonymous ");
    strcat(ftpCommand, ipAddrStr);
    strcat(ftpCommand, " ");
    strcat(ftpCommand, destFile);
    strcat(ftpCommand, " ");
    strcat(ftpCommand, sessionData.requestInfo.fileServerPath);
    /* Check if "/" should be added */
    pch = strrchr(sessionData.requestInfo.fileServerPath,'/');
    if (pch == NULL)
    {
        /* No "/" found */
        strcat(ftpCommand,"/");
    }
    else
    {
        if (strlen(sessionData.requestInfo.fileServerPath) != (pch-sessionData.requestInfo.fileServerPath+1U))
        {
            /* Last character is not "/", add character */
            strcat(ftpCommand,"/");
        }
    }
    strcat(ftpCommand, sessionData.requestInfo.fileName);
    DebugError1("FTP command (%s)", ftpCommand);
    sysResult = system(ftpCommand);
    if (sysResult == 0)
    {
        result = dleds_checkCRC(destFile, sessionData.requestInfo.fileCRC, 0x0);
        if (result != DLEDS_OK)
        {
            result = DLEDS_CRC_ERROR;
        }
    }
    else
    {
        DebugError2("FTP command failed (%s) returned (%d)", ftpCommand, sysResult);
        result = DLEDS_ERROR;
    }


    /******************************************************************************
    *   ADD CODE FOR PLATFORM (END)
    ******************************************************************************/

    return result;
}


/*******************************************************************************
 *
 * Function name: dledsPlatformSetDledsTempPath
 *
 * Abstract:      Creates path to an existing directory that could be used as
 *                temporary DLEDS directory where ED package will be stored
 *                and unpacked.
 *
 * Return value:  DLEDS_OK      - if OK
 *                DLEDS_ERROR   - otherwise
 *
 * Globals:       None
 */
DLEDS_RESULT dledsPlatformSetDledsTempPath(
    char* dledsDirectoryPath)           /* IN/OUT: Directory path */
{
    DLEDS_RESULT    result = DLEDS_ERROR;

    if (dledsDirectoryPath == NULL)
    {
        return DLEDS_ERROR;
    }

    /* Initiate directory path to empty string */
   // strcpy(dledsDirectoryPath, "");

    /******************************************************************************
    *   ADD CODE FOR PLATFORM (START)
    ******************************************************************************/
    #ifdef PRINTCONSOLE
    printf("Function %s \n",__FUNCTION__);
    #endif

    result = dleds_setLinuxDledsTempPath(dledsTempDirectory);

    if (result == DLEDS_ERROR)
    {
        /* Use flash file system for temporary DLEDS directory */
        strcpy(dledsDirectoryPath, DLEDS_ROOT_DIRECTORY);
        result = DLEDS_OK;
        DebugError0("dledsPlatformSetDledsTempPath: ERROR so Use flash file system dir=/");
    }
    /******************************************************************************
    *   ADD CODE FOR PLATFORM (END)
    ******************************************************************************/

    return result;
}


/*******************************************************************************
 *
 * Function name: dledsPlatformCalculateFreeDiskSpace
 *
 * Abstract:      This function calculates the size in kByte of free disk space
 *                available in file system.
 *
 * Return value:  X kByte      - Disk space available
 *                0            - Error
 *
 * Globals:       -
 */
UINT32 dledsPlatformCalculateFreeDiskSpace(
    char*  filePath)                    /* IN: path to file system */
{
    UINT32 freeDiskSpace = 0;

    if (filePath == NULL)
    {
        return 0;
    }

    /******************************************************************************
    *   ADD CODE FOR PLATFORM (START)
    ******************************************************************************/
    #ifdef PRINTCONSOLE
    printf("Function %s \n",__FUNCTION__);
    #endif

    struct statvfs sfs;
    unsigned long long result = 0;

    if ( 0 == statvfs( filePath,&sfs ) )
    {
        result = (unsigned long long)sfs.f_bsize * sfs.f_bfree;
    }
    else
    {
        if (EINTR == errno)   /* interrupted system call, try again */
        {
            if (0 == statvfs(filePath,&sfs))
            {
                result = (unsigned long long) sfs.f_bsize * sfs.f_bfree;
            }
            else
            {
                DebugError1("Could not calculate the amount of free disk space on (%s)", filePath);
                return DLEDS_ERROR;
            }
        }
    }
    freeDiskSpace = (UINT32)result/(1024); /* size in KBytes */
    /******************************************************************************
    *   ADD CODE FOR PLATFORM (END)
    ******************************************************************************/

    return freeDiskSpace;
}

/*******************************************************************************
 *
 * Function name: dledsPlatformGetSwInfoXml
 *
 * Abstract:      This function returns SW info in an XML structure.
 *
 * Return value:  DLEDS_OK      - SW info found
 *                DLEDS_ERROR   - No SW info found
 *
 * Globals:       -
 */

char dststring[RESULTBASH_SIZESTRING];
int  blen;

DLEDS_RESULT dledsPlatformGetSwInfoXml(
    UINT32  *pActSize,          /* OUT: Pointer to buffer size */
    char    **ppInfoBuffer)     /* OUT: Pointer to buffer pointer */
{
    DLEDS_RESULT    result = DLEDS_ERROR;
    FILE    *fp;

    if ((pActSize == NULL) && (ppInfoBuffer == NULL))
    {
        result = DLEDS_ERROR;
    }
    /******************************************************************************
    *   ADD CODE FOR PLATFORM (START)
    ******************************************************************************/
    //#ifdef PRINTCONSOLE
    printf("Function %s \n",__FUNCTION__);
    //#endif
/*
    char dststring[RESULTBASH_SIZESTRING];
    int len=0;
    */
    /* FILIPPO */
    memset (dststring,'\0',RESULTBASH_SIZESTRING);

    fp = fopen("/tmp/DledsHwSw.xml","r");
    if (fp == NULL)
        return DLEDS_ERROR;
    blen = fread(dststring,1,RESULTBASH_SIZESTRING,fp)  ;
    fclose(fp);

    *ppInfoBuffer = dststring;
    *pActSize = blen;

    printf("len = %d\n%s\n",*pActSize,*ppInfoBuffer);
    result = DLEDS_OK;
    /**/
    /******************************************************************************
    *   ADD CODE FOR PLATFORM (END)
    ******************************************************************************/
    return result;
}

/*******************************************************************************
 *
 * Function name: dledsPlatformGetHwInfoXml
 *
 * Abstract:      This function returns HW info in an XML structure.
 *
 * Return value:  DLEDS_OK      - HW info found
 *                DLEDS_ERROR   - No HW info found
 *
 * Globals:       -
 */
DLEDS_RESULT dledsPlatformGetHwInfoXml(
    UINT32  *pActSize,          /* OUT: Pointer to buffer size */
    char    **ppInfoBuffer)     /* OUT: Pointer to buffer pointer */
{
    DLEDS_RESULT    result = DLEDS_ERROR;
    FILE    *fp;

    if ((pActSize == NULL) && (ppInfoBuffer == NULL))
    {
        result = DLEDS_ERROR;
    }

    /******************************************************************************
    *   ADD CODE FOR PLATFORM (START)
    ******************************************************************************/
    //#ifdef PRINTCONSOLE
    printf("Function %s \n",__FUNCTION__);
    //#endif

    /* FILIPPO */
    memset (dststring,'\0',RESULTBASH_SIZESTRING);

    fp = fopen("/tmp/HWRev.xml","r");
    if (fp == NULL)
        return DLEDS_ERROR;
    blen = fread(dststring,1,RESULTBASH_SIZESTRING,fp)  ;
    fclose(fp);

    *ppInfoBuffer = dststring;
    *pActSize = blen;

    printf("len = %d\n%s\n",*pActSize,*ppInfoBuffer);
    result = DLEDS_OK;
    /**/
    /******************************************************************************
    *   ADD CODE FOR PLATFORM (END)
    ******************************************************************************/
    return result;
}

/*******************************************************************************
 *
 * Function name: dledsPlatformReleaseInfoXml
 *
 * Abstract:      This function releases the buffer allocated for SW or HW
 *                XML info.
 *
 * Return value:  -
 *
 * Globals:       -
 */
void dledsPlatformReleaseInfoXml(
    char    *pInfoBuffer)     /* IN : Pointer to buffer pointer */
{

    if (pInfoBuffer == NULL)
    {
        return;
    }

    /******************************************************************************
    *   ADD CODE FOR PLATFORM (START)
    ******************************************************************************/
    //#ifdef PRINTCONSOLE
    printf("Function %s \n",__FUNCTION__);
    //#endif
/*
   char dststring[RESULTBASH_SIZESTRING];
    int len=0;

    memset (dststring,'\0',RESULTBASH_SIZESTRING);



    len=strlen("ls -l");  // xml for HW and SW info
    bash_exec(len,"ls -l", dststring);
    */

    /******************************************************************************
    *   ADD CODE FOR PLATFORM (END)
    ******************************************************************************/
    return;
}


/*******************************************************************************
 *
 * Function name: dledsPlatformGetHwInfo
 *
 * Abstract:      This function returns HW info.
 *
 * Return value:  DLEDS_OK      - HW info found
 *                DLEDS_ERROR   - No HW info found
 *
 * Globals:       -
 */
 /* FILIPPO */
DLEDS_RESULT dledsPlatformGetHwInfo(
    char* unit_type,            /* OUT: Pointer to unit type string (16 characters, including '\0') */
    char* serial_number,        /* OUT: Pointer to serial number string (16 characters, including '\0') */
    char* delivery_revision)    /* OUT: Pointer to delivered revision (16 characters, including '\0') */
{
    DLEDS_RESULT    result = DLEDS_OK;

    if ((unit_type == NULL) || (serial_number == NULL) || (delivery_revision == NULL))
    {
        return DLEDS_ERROR;
    }

    /* Initiate output variables to empty string */
    strcpy(unit_type, "Lcd");
    strcpy(serial_number, "12345678");
    strcpy(delivery_revision, "1.0.0");

    /******************************************************************************
    *   ADD CODE FOR PLATFORM (START)
    ******************************************************************************/
    #ifdef PRINTCONSOLE
    printf("Function %s \n",__FUNCTION__);
    #endif
    /******************************************************************************
    *   ADD CODE FOR PLATFORM (END)
    ******************************************************************************/

    return result;
}



/*******************************************************************************
 *
 * Function name: dledsPlatformGetEdIdentSwVersions
 *
 * Abstract:      This function returns SW versions for ED Identification
 *                message. The format of the version is 0xVVRRUUEE where
 *                VV = version  (00: No version info available)
 *                RR = release
 *                UU = update
 *                EE = evolution
 *
 *
 * Return value:  -
 *
 * Globals:       -
 */
void dledsPlatformGetEdIdentSwVersions(
    UINT32* btDluVersion,        /* OUT: Pointer to BT version */
    UINT32* cuDluVersion,        /* OUT: Pointer to CU version */
    UINT32* osDluVersion,        /* OUT: Pointer to OS version */
    UINT32* blcfgUluVersion,     /* OUT: Pointer to Boot Loader Configuration version */
    UINT32* ubootUluVersion)     /* OUT: Pointer to UBOOT version */
{

    if ((btDluVersion == NULL) || (cuDluVersion == NULL) ||  (osDluVersion == NULL) ||
        (blcfgUluVersion == NULL)  || (ubootUluVersion == NULL))
    {
        return;
    }

    /* Initiate output variables */
    *btDluVersion = 0x00FFFFFF;
    *cuDluVersion = 0x00FFFFFF;
    *osDluVersion = 0x00FFFFFF;
    *blcfgUluVersion = 0x00FFFFFF;
    *ubootUluVersion = 0x00FFFFFF;

    /******************************************************************************
    *   ADD CODE FOR PLATFORM (START)
    ******************************************************************************/
    #ifdef PRINTCONSOLE
    printf("Function %s \n",__FUNCTION__);
    #endif
    /******************************************************************************
    *   ADD CODE FOR PLATFORM (END)
    ******************************************************************************/

}



/*******************************************************************************
 *
 * Function name: dledsPlatformCreateSubDirectory
 *
 * Abstract:      This function creates a sub directory if it does not exist.
 *
 * Return value:  DLEDS_OK      -  Directory exists or has been created
 *                DLEDS_ERROR   -  Could not create directory
 *
 * Globals:
 */
DLEDS_RESULT dledsPlatformCreateSubDirectory(
    char* dirName)                      /* IN: Full path to directory */
{
    DLEDS_RESULT    result = DLEDS_ERROR;

    struct stat     fileStat;

    if (dirName != NULL)
    {
        /******************************************************************************
        *   ADD CODE FOR PLATFORM (START)
        ******************************************************************************/
    #ifdef PRINTCONSOLE
    printf("Function %s \n",__FUNCTION__);
    #endif
     /* Check if directory exists */
        if (stat(dirName, &fileStat) != 0)
        {
            /* Create directory */
            if (mkdir(dirName,S_IRWXU | S_IRWXG | S_IRWXO) != 0)
            {
            printf("mkdir*************** dirname:%s *************************\n",dirName);
            sleep(5);
                DebugError1("Failed to create directory (%s)", dirName);
                result=DLEDS_ERROR;
            }
            else
            {
                result=DLEDS_OK;
            }
        }
        else
        {
            result=DLEDS_OK;
        }
        /******************************************************************************
        *   ADD CODE FOR PLATFORM (END)
        ******************************************************************************/
    }


    return result;
}

/*******************************************************************************
 *
 * Function name: dledsPlatformCleanupEDSP2
 *
 * Abstract:      This function deletes all DLU:s that are part of a
 *                previously installed EDSP version 2.
 *
 * Return value:  DLEDS_OK      -  Installed BT package deleted
 *                DLEDS_ERROR   -  Could not delete installed BT package
 *
 * Globals:       None
 */
DLEDS_RESULT dledsPlatformCleanupEDSP2(
    char* edspName)         /* IN: EDSP to be removed from target */
{
    DLEDS_RESULT        result = DLEDS_OK;

    if (edspName == NULL)
    {
        result = DLEDS_ERROR;
    }

    /******************************************************************************
    *   ADD CODE FOR PLATFORM (START)
    ******************************************************************************/
    #ifdef PRINTCONSOLE
    printf("Function %s \n",__FUNCTION__);
    #endif

    /******************************************************************************
    *   ADD CODE FOR PLATFORM (END)
    ******************************************************************************/

    return result;
}


/*******************************************************************************
 *
 * Function name: dledsPlatformCleanupBT
 *
 * Abstract:      This function deletes all DLU:s that are part of the
 *                previously installed BT package.
 *
 * Return value:  DLEDS_OK      -  Installed BT package deleted
 *                DLEDS_ERROR   -  Could not delete installed BT package
 *
 * Globals:       None
 */
DLEDS_RESULT dledsPlatformCleanupBT(
    void)                               /* IN: None */
{
    DLEDS_RESULT        result = DLEDS_ERROR;

    /******************************************************************************
    *   ADD CODE FOR PLATFORM (START)
    ******************************************************************************/
    #ifdef PRINTCONSOLE
    printf("Function %s \n",__FUNCTION__);
    #endif
    /******************************************************************************
    *   ADD CODE FOR PLATFORM (END)
    ******************************************************************************/

    return result;
}


/*******************************************************************************
 *
 * Function name: dledsPlatformCleanupCU
 *
 * Abstract:      This function deletes all DLU:s that are part of the
 *                previously installed CU package.
 *
 * Return value:  DLEDS_OK      -  Installed CU package deleted
 *                DLEDS_ERROR   -  Could not delete installed CU package
 *
 * Globals:       None
 */
DLEDS_RESULT dledsPlatformCleanupCU(
    void)                               /* IN: None */
{
    DLEDS_RESULT        result = DLEDS_ERROR;

    /******************************************************************************
    *   ADD CODE FOR PLATFORM (START)
    ******************************************************************************/
    #ifdef PRINTCONSOLE
    printf("Function %s \n",__FUNCTION__);
    #endif
    /******************************************************************************
    *   ADD CODE FOR PLATFORM (END)
    ******************************************************************************/

    return result;
}


/*******************************************************************************
 *
 * Function name: dledsPlatformRemovePackages
 *
 * Abstract:      This function deletes all DLU:s that has been installed as
 *                part of an edsp package.
 *
 * Return value:  DLEDS_OK      -  Installed edsp2 packages deleted
 *                DLEDS_ERROR   -  Could not delete installed edsp2 packages
 *
 * Globals:       None
 */
DLEDS_RESULT dledsPlatformRemovePackages(
    void)                               /* IN: None */
{
    DLEDS_RESULT        result = DLEDS_ERROR;

    /******************************************************************************
    *   ADD CODE FOR PLATFORM (START)
    ******************************************************************************/
    #ifdef PRINTCONSOLE
    printf("Function %s \n",__FUNCTION__);
    #endif
    /******************************************************************************
    *   ADD CODE FOR PLATFORM (END)
    ******************************************************************************/

    return result;
}

/*******************************************************************************
 *
 * Function name: dledsPlatformInstallDlu2
 *
 * Abstract:      This function installs a DLU from a version2 package on target
 *                and verifies that the DLU has been installed correctly.
 *
 * Return value:  DLEDS_OK      -  DLU successfully installed
 *                DLEDS_ERROR   -  Error during installation of DLU
 *
 * Globals:       None
 */
DLEDS_RESULT dledsPlatformInstallDlu2(
    TYPE_DLEDS_DLU_DATA2* pDluData)  /*  IN: Pointer to DLU file info */
{
    DLEDS_RESULT  result = DLEDS_OK;
    char    cmd[255];

    if (pDluData == NULL)
    {
        printf("DLEDS_ERROR %s \n",__FUNCTION__);
        return DLEDS_ERROR;
    }

    /******************************************************************************
    *   ADD CODE FOR PLATFORM (START)
    ******************************************************************************/
    //#ifdef PRINTCONSOLE
    printf("Function %s, file is %s/%s\n",__FUNCTION__, pDluData->dluFilePath, pDluData->dluFileName);
    sprintf(cmd,"cp %s/%s /tmp/downloaded_file\n",pDluData->dluFilePath, pDluData->dluFileName);
    system(cmd);
    //#endif

    /******************************************************************************
    *   ADD CODE FOR PLATFORM (END)
    ******************************************************************************/
    return result;
}


/*******************************************************************************
 *
 * Function name: dledsPlatformInstallDlu
 *
 * Abstract:      This function installs a DLU on target and verifies that the
 *                DLU has been installed correctly.
 *
 * Return value:  DLEDS_OK      -  DLU successfully installed
 *                DLEDS_ERROR   -  Error during installation of DLU
 *
 * Globals:       None
 */
DLEDS_RESULT dledsPlatformInstallDlu(
    TYPE_DLEDS_DLU_DATA* pDluData)  /*  IN: Pointer to DLU file info */
{
    DLEDS_RESULT        result = DLEDS_ERROR;

    if (pDluData == NULL)
    {
        return DLEDS_ERROR;
    }

    /******************************************************************************
    *   ADD CODE FOR PLATFORM (START)
    ******************************************************************************/
    //#ifdef PRINTCONSOLE
    printf("Function %s \n",__FUNCTION__);
    //#endif
    /******************************************************************************
    Install payload data from DLU file on target
    ******************************************************************************/

    /******************************************************************************
    Verify that DLU file has been installed correctly on target
    ******************************************************************************/

    /******************************************************************************
    Should meta data be stored for DLU file
    ******************************************************************************/

    /******************************************************************************
    *   ADD CODE FOR PLATFORM (END)
    ******************************************************************************/

    return result;
}


/*******************************************************************************
 *
 * Function name: dledsPlatformUnpackTarFile
 *
 * Abstract:      This function unpack a TAR file. The files within the TAR file
 *                are extracted to the directory pointed out by input parameter.
 *                The TAR file is deleted when it has been unpacked.
 *
 * Return value:  DLEDS_OK      -  TAR file extracted successfully
 *                DLEDS_ERROR   -  TAR file not extracted
 *
 * Globals:       -
 */
DLEDS_RESULT dledsPlatformUnpackTarFile(
    char*   fileName,                   /* IN: TAR file with full path */
    char*   destinationDir,             /* IN: Directory to extract to */
    UINT8   compressed)                 /* IN: 1= TAR file is compressed */
{
    DLEDS_RESULT    result = DLEDS_OK;

    if ((fileName == NULL) && (destinationDir == NULL))
    {
        printf("%s : fileName == NULL destinationDir == NULL\n",__FUNCTION__);
        return DLEDS_ERROR;
    }
    else
    {
        /******************************************************************************
        *   ADD CODE FOR PLATFORM (START)
        ******************************************************************************/
        #ifdef PRINTCONSOLE
        printf("Function %s \n",__FUNCTION__);
        #endif
        system ("cp -r /dleds /tmp/dledsdwnld\n");

        (void)compressed; /* To avoid compiler warning */

        /******************************************************************************
        Unpack TAR file to destination directory
        ******************************************************************************/
        DebugError2("***PLATFORM: Unpack TAR file(%s) in directory (%s)", fileName, destinationDir);
        DebugError1("***PLATFORM: TAR file compressed flag is (%d)", compressed);
        char            tarCommand[256];
        int             sysResult;

        /* Add destination directory to tar command */
        strcpy(tarCommand, "tar -C ");
        strcat(tarCommand, destinationDir);

        /* Check if TAR file is compressed */
        if (compressed == 1)
        {
            strcat(tarCommand, " -xzvf ");
        }
        else
        {
            strcat(tarCommand, " -xvf ");
        }

        strcat(tarCommand, fileName);
        sysResult = system(tarCommand);
        if (sysResult != 0)
        {
            DebugError2("Tar command failed (%s) returned (%d)", tarCommand, sysResult);
            result = DLEDS_ERROR;
        }
        remove(fileName);
        /******************************************************************************
        *   ADD CODE FOR PLATFORM (END)
        ******************************************************************************/
    }
    return result;
}


/*******************************************************************************
 *
 * Function name: dleds_initialize
 *
 * Abstract:      This function contains the dleds initialization steps.
 *
 * Return value:  DLEDS_OK
 *                DLEDS_ERROR
 *
 * Globals:       appResultCode
 *                sessionData
 */
DLEDS_RESULT dleds_initialize(
    void)
{
    DLEDS_RESULT result = DLEDS_OK;
    int iptResult = IPT_OK;
    static int initStep = STEP1;
    static int retryCounter = 0;

    #ifdef PRINTCONSOLE
    printf("Function %s \n",__FUNCTION__);
    #endif

    /*
     *  STEP1: Handle start-up of IPTCom in a multi-process Linux based system
     */
    if (initStep == STEP1)
    {

        /*
         * IPTCom used in a single process environment, single address space
         */

        /* Continue with next step */
        initStep = STEP2;

        /*
         * IPTCom used in a multiple process, multiple address space environment
         */

        /*
         * The IPTCom in a secondary process is started by calling the method
         * IPTCom_MPattach(). Attaches to an IPTCom already started with
         * IPTCom_prepareInit by the primary process in a Linux multi-processing
         * system. For more information on this topic see IPTCom User and
         * Integration Manual chapter "How to use IPTCom".
         *
         *
         * Example code:
         * iptResult = IPTCom_MPattach();
         * if (iptResult == IPT_OK)
         * {
         *     DebugError0("IPTCom Shared Memory Attached");
         *     // Continue with next step
         *     retryCounter = 0;
         *     initStep = STEP2;
         * }
         * else
         * {
         *     DebugError2("IPTCom_MPattach() returned error ((0x%x) %s)",
         *         iptResult,
         *         IPTCom_getErrorString(iptResult));
         *
         *     if (retryCounter > MAX_NO_OF_RETRIES)
         *     {
         *         // Maximum number of retries exceeded, stop dleds daemon
         *         initStep = STEP1;
         *         retryCounter = 0;
         *         result = DLEDS_NO_OF_RETRIES_EXCEEDED;
         *         return result;
         *      }
         *      else
         *      {
         *          // wait and then try to run this step again
         *          retryCounter++;
         *          result = DLEDS_ERROR;
         *          IPTVosTaskDelay(SHARED_MEMORY_ATTACHED_WAIT);
         *      }
         * }
         *
         */
    }

    /*
     *  STEP2:  Check that IPTCom and TDC are initiated
     */
    if (initStep == STEP2)
    {
        iptResult = IPTCom_getStatus();
        if (iptResult == IPTCOM_RUN)
        {
            DebugError0("IPTCom initiated and TDC has got configuration data from IPTDir");
            /* continue with next step */
            retryCounter = 0;
            initStep = STEP3;
        }
        else
        {
            if (iptResult == IPTCOM_TDC_NOT_CONFIGURED)
            {
                DebugError0("IPTCom initiated and TDC is waiting for configuration data from IPTDir");
            }
            else if (iptResult == IPTCOM_NOT_INITIATED)
            {
                DebugError0("IPTCom or TDC not initiated");
            }
            else
            {
                DebugError1("IPTCom_getStatus() returned unknown error code (0x%x)",
                    iptResult);
            }

            if (retryCounter > MAX_NO_OF_GET_STATUS_RETRIES)
            {
                /* Maximum number of retries exceeded, stop dleds daemon */
                initStep = STEP1;
                retryCounter = 0;
                result = DLEDS_NO_OF_RETRIES_EXCEEDED;
                return result;
            }
            else
            {
                /* wait and then try to run this step again */
                retryCounter++;
                result = DLEDS_ERROR;
                IPTVosTaskDelay(IPTCOM_GET_STATUS_WAIT);
            }

        }
    }

    /*
     *  STEP3:  Create a MD queue to be able to receive the download request messages
     */
    if(initStep == STEP3)
    {
        requestQueueId = MDComAPI_queueCreate(DLEDS_QUEUE_SIZE, DLEDS_REQ_QUEUE_NAME);
        if (requestQueueId != (MD_QUEUE) NULL)
        {
            DebugError0("DLEDS request receive queue created successfully");

            /* continue with next step */
            retryCounter = 0;
            initStep = STEP4;
        }
        else
        {
            DebugError0("MDComAPI_queueCreate() failed to create request queue");

            if (retryCounter > MAX_NO_OF_RETRIES)
            {
                /* Maximum number of retries exceeded, stop dleds daemon */
                initStep = STEP1;
                retryCounter = 0;
                result = DLEDS_NO_OF_RETRIES_EXCEEDED;
                return result;
            }
            else
            {
                /* wait and then try to run this step again */
                retryCounter++;
                result = DLEDS_ERROR;
                IPTVosTaskDelay(QUEUE_CREATE_WAIT);
            }
        }
    }

    /*
     *  STEP4:  Create a MD queue to be able to receive the result messages
     *          on the response and status messages
     */
    if(initStep == STEP4)
    {
        resultQueueId = MDComAPI_queueCreate(DLEDS_QUEUE_SIZE, DLEDS_RES_QUEUE_NAME);
        if (resultQueueId != (MD_QUEUE) NULL)
        {
            DebugError0("DLEDS result receive queue created successfully");

            /* continue with next step */
            retryCounter = 0;
            initStep = STEP4_1;
        }
        else
        {
            DebugError0("MDComAPI_queueCreate() failed to create result queue");

            if (retryCounter > MAX_NO_OF_RETRIES)
            {
                /* Maximum number of retries exceeded, stop dleds daemon */
                initStep = STEP1;
                retryCounter = 0;
                result = DLEDS_NO_OF_RETRIES_EXCEEDED;
                return result;
            }
            else
            {
                /* wait and then try to run this step again */
                retryCounter++;
                result = DLEDS_ERROR;
                IPTVosTaskDelay(QUEUE_CREATE_WAIT);
            }
        }
    }

    /*
     *  STEP4_1:  Create a MD queue to be able to receive echo messages
     */
    if(initStep == STEP4_1)
    {
        echoQueueId = MDComAPI_queueCreate(DLEDS_QUEUE_SIZE, DLEDS_ECHO_QUEUE_NAME);
        if (echoQueueId != (MD_QUEUE) NULL)
        {
            DebugError0("DLEDS echo receive queue created successfully");

            /* continue with next step */
            retryCounter = 0;
            initStep = STEP4_2;
        }
        else
        {
            DebugError0("MDComAPI_queueCreate() failed to create echo queue");

            if (retryCounter > MAX_NO_OF_RETRIES)
            {
                /* Maximum number of retries exceeded, stop dleds daemon */
                initStep = STEP1;
                retryCounter = 0;
                result = DLEDS_NO_OF_RETRIES_EXCEEDED;
                return result;
            }
            else
            {
                /* wait and then try to run this step again */
                retryCounter++;
                result = DLEDS_ERROR;
                IPTVosTaskDelay(QUEUE_CREATE_WAIT);
            }
        }
    }

    /*
     *  STEP4_2:  Create a MD queue to be able to receive progress response messages
     */
    if(initStep == STEP4_2)
    {
        progressQueueId = MDComAPI_queueCreate(DLEDS_QUEUE_SIZE, DLEDS_PROGRESS_QUEUE_NAME);
        if (progressQueueId != (MD_QUEUE) NULL)
        {
            DebugError0("DLEDS progress receive queue created successfully");

            /* continue with next step */
            retryCounter = 0;
            initStep = STEP5;
        }
        else
        {
            DebugError0("MDComAPI_queueCreate() failed to create response queue");

            if (retryCounter > MAX_NO_OF_RETRIES)
            {
                /* Maximum number of retries exceeded, stop dleds daemon */
                initStep = STEP1;
                retryCounter = 0;
                result = DLEDS_NO_OF_RETRIES_EXCEEDED;
                return result;
            }
            else
            {
                /* wait and then try to run this step again */
                retryCounter++;
                result = DLEDS_ERROR;
                IPTVosTaskDelay(QUEUE_CREATE_WAIT);
            }
        }
    }

    /*
     *  STEP5:  Add IPTcom configuration data neccessary for DLEDS
     */
    if(initStep == STEP5)
    {
        iptResult = dleds_add_ipt_config();
        if (iptResult == IPT_OK )
        {
            DebugError0("IPTCom configuration added successfully");
            initStep = STEP6;
        }
        else if (iptResult == (int)IPT_TAB_ERR_EXISTS)
        {
            DebugError0("IPTCom configuration already loaded");
            initStep = STEP6;
        }
        else
        {
            DebugError2("IPTCom_addConfig() returned error ((0x%x) %s)",
                iptResult,
                IPTCom_getErrorString(iptResult));

            if (retryCounter > MAX_NO_OF_RETRIES)
            {
                /* Maximum number of retries exceeded, stop dleds daemon */
                initStep = STEP1;
                retryCounter = 0;
                return DLEDS_NO_OF_RETRIES_EXCEEDED;
            }
            else
            {
                /* wait and then try to run this step again */
                retryCounter++;
                IPTVosTaskDelay(IPTCOM_ADD_CONFIG_WAIT);
            }
        }
    }

    /*
     *  STEP6:  Add a listener for message data with the given destination URI
     */
    if(initStep == STEP6)
    {
        /* Add URI listener for DLEDS */
        iptResult = MDComAPI_uriListener(
                        requestQueueId,         /* Queue identification */
                        (IPT_REC_FUNCPTR) 0,    /* Pointer to callback function (NOT USED) */
                        (void *)0,              /* Caller reference value (NOT USED)*/
                        0,                      /* ComId */
                        0,                      /* Destination Id */
                        DLEDS_SERVICE_NAME,     /* Pointer to destination URI string */
                        (void *)0);             /* Redundancy reference pointer (NOT USED)*/
        if(iptResult == IPT_OK)
        {
            DebugError0("URI Listener added successfully");
            retryCounter = 0;
            initStep = STEP7;
        }
        else
        {
            DebugError2("MDComAPI_uriListener() returned error ((0x%x) %s)",
                iptResult,
                IPTCom_getErrorString(iptResult));

            if (retryCounter > MAX_NO_OF_RETRIES)
            {
                /* Maximum number of retries exceeded, stop dleds daemon */
                initStep = STEP1;
                retryCounter = 0;
                return DLEDS_NO_OF_RETRIES_EXCEEDED;
            }
            else
            {
                /* wait and then try to run this step again */
                retryCounter++;
                IPTVosTaskDelay(ADD_LISTENER_WAIT);
            }
        }

    }

    /*
     *  STEP7:  Register a publisher of a ComId.
     *          The data for the ComId is sent cyclic with the configured cycle
     *          time. No data is send before it has been updated the first time
     *          by the application calling PDComAPI_put() and PDComAPI_source().
     */
    if(initStep == STEP7)
    {
        pdHandle = PDComAPI_pub(DLEDS_SCHEDULE_GROUP_ID, EDIDENT_COMID, 0, EDIDENT_DESTURI);
        if(pdHandle != (PD_HANDLE)NULL)
        {
            DebugError1("Publisher registered for ComID %d",
                EDIDENT_COMID);
            initStep = STEP8;
        }
        else
        {
            DebugError0("PDComAPI_pub() failed");

            if (retryCounter > MAX_NO_OF_RETRIES)
            {
                /* Maximum number of retries exceeded, stop dleds daemon */
                initStep = STEP1;
                retryCounter = 0;
                result = DLEDS_NO_OF_RETRIES_EXCEEDED;
                return result;
            }
            else
            {
                /* wait and then try to run this step again */
                retryCounter++;
                IPTVosTaskDelay(REGISTER_PUBLISHER_WAIT);
            }
        }
    }

    if(initStep == STEP8)
    {
        /* All steps of initialization executed successfully */
        /* reset step counter, so that reinitialization works properly*/
        initStep = STEP1;
        return DLEDS_OK;
    }
    else
    {
        /* This will cause a retry of current step when this function is called again */
        DebugError0("Return ERROR from dleds_initialize and retry current STEPx");
        return DLEDS_ERROR;
    }
}

/*******************************************************************************
 *
 * Function name: dledsPlatformInspectPackage
 *
 * Abstract:      This function checks if this is a special package that must be
 *                handled by the target. If so, the return code, DLEDS_STATE_CHANGE,
 *                will indicate that the package must be handled by the target
 *                itself. The DLEDS will enter a DLEDS IDLE MODE and wait until
 *                the target will signal that the package has been processed.
 *
 * Return value:  DLEDS_OK
 *                DLEDS_ERROR
 *                DLEDS_STATE_CHANGE
 *
 * Globals:       currentDir
 */
DLEDS_RESULT dledsPlatformInspectPackage(
    char    *downloadFileName)
{
    DLEDS_RESULT     result = DLEDS_OK;

    if (downloadFileName == NULL)
    {
        result = DLEDS_ERROR;
    }

    /******************************************************************************
    *   ADD CODE FOR PLATFORM (START)
    ******************************************************************************/
    #ifdef PRINTCONSOLE
    printf("Function %s \n",__FUNCTION__);
    #endif
    /******************************************************************************
    *   ADD CODE FOR PLATFORM (END)
    ******************************************************************************/
    return result;
}

/*******************************************************************************
 *
 * Function name: dledsPlatformPassPackageProcessing
 *
 * Abstract:      This function activates a process on target will process the
 *                package.
 *
 * Return value:  DLEDS_OK
 *                DLEDS_ERROR
 *
 * Globals:       -
 */
DLEDS_RESULT dledsPlatformPassPackageProcessing(void)
{
     DLEDS_RESULT result = DLEDS_ERROR;

    /******************************************************************************
    *   ADD CODE FOR PLATFORM (START)
    ******************************************************************************/
    #ifdef PRINTCONSOLE
    printf("Function %s \n",__FUNCTION__);
    #endif
    /******************************************************************************
    *   ADD CODE FOR PLATFORM (END)
    ******************************************************************************/
    return result;
}

/*******************************************************************************
 *
 * Function name: dledsPlatformHandleIdleMode
 *
 * Abstract:      This function handles the DLEDS IDLE MODE when processing of the
 *                received package is done by the target. Dleds will cyclically call
 *                this function while in DLEDS IDLE MODE and wait for a signal that
 *                the target is ready with the processing of the package. It is
 *                up to the user to define how the signalling is done.
 *
 * Return value:  SEND_STATUS - When processing of package is ready
 *                IDLE_MODE   - While not ready with processing of package
 *
 * Globals:
 */
static STATE dledsPlatformHandleIdleMode(void)
{
    STATE result = IDLE_MODE;
    /******************************************************************************
    *   ADD CODE FOR PLATFORM (START)
    ******************************************************************************/
    #ifdef PRINTCONSOLE
    printf("Function %s \n",__FUNCTION__);
    #endif
    IPTVosTaskDelay(1000);
    /******************************************************************************
    *   ADD CODE FOR PLATFORM (END)
    ******************************************************************************/
    return result;
}


/*******************************************************************************
 *
 * Function name: dleds_thread
 *
 * Abstract:      This function is the main entry function for DLEDS that
 *                consists of a state machine that will loop forever.
 *
 * Return value:  None
 *
 * Globals:       dledsTempDirectory
 */
void dleds_thread(void)
{
    static int done = FALSE;
    DLEDS_RESULT result = DLEDS_OK;
    int operationMode;
    FILE* dledsDebugFileHandle = 0;


    /* Make sure that ROOT directory for DLEDS is available */
    if (createSubDirectory(DLEDS_ROOT_DIRECTORY) != DLEDS_OK)
    {
        /* This is the directory used by the LOG function and */
        /* possibly as root for DLEDS temporary directory. Do */
        /* not continue */
        printf("dleds_thread - createSubDirectory %s ERROR\n",DLEDS_ROOT_DIRECTORY);
        sleep(5);
        return;
    }

    /* Check if DLEDS logging is activated */
    /* Debug logging should be activated if file exists "r" */
    /* GIO modificato con opzione "a" per la creazione di default */
    if((dledsDebugFileHandle = fopen(DLEDS_DEBUG_ACTIVE, "a")) == NULL)
    {
        /* debug activate file does not exist, Debug not active */
        dledsDebugOn = 0;
        printf("dleds_thread - NO DEBUG\n");

    }
    else
    {
        /* debug activate file exist, Debug active */
        printf("dleds_thread - DEBUG\n");
        dledsDebugOn = 1;
        fclose(dledsDebugFileHandle);

    }

    /* Initiate variable for DLEDS temporary directory */
    strcpy(dledsTempDirectory, "");

    printf("DLEDS version   = %s", SW_VERSION_STRING);
    DebugError1("DLEDS version   = %s", SW_VERSION_STRING);

    while(!done)
    {
        #ifdef STATEMACHINE
        printf("dleds_thread state:%s\n",statemachine[DLEDS_STATE]);
        #endif
        switch(DLEDS_STATE)
        {
            case UNDEFINED:
            {
                IPTVosTaskDelay(5000);
                result = dleds_initialize();
                if(result == DLEDS_OK)
                {
                    DebugError0("Initialization passed");
                    /* continue with next state */
                    DLEDS_STATE = INITIALIZED;
                }
                else
                {
                    if (result == DLEDS_NO_OF_RETRIES_EXCEEDED)
                    {
                        /* initialization for DLEDS daemon failed */
                        DebugError0("Initialization failed");
                        DLEDS_STATE = STOP;
                    }
                }
                /* stay in this state */
                break;
            }
            case INITIALIZED:
            {
                operationMode = dledsPlatformGetOperationMode();
                if (operationMode == DLEDS_OSDOWNLOAD_MODE)
                {
                    /* Started in mode that accepts installation of package */
                    DebugError0("Started in OS DOWNLOAD mode");
                    result = dleds_checkDownloadModeStatus();
                    if (result == DLEDS_OK)
                    {
                        DLEDS_STATE = DOWNLOAD_MODE;
                    }
                    else
                    {
                        /*
                         * Error occured that makes it impossible to
                         * continue with the current download request.
                         * Reset to RUN mode and wait for next request.
                         */
                        DebugError0("Session information could not be retrieved from file");
                        DebugError0("Could not continue in download mode, go to run mode");
                        DLEDS_STATE = RESET_MODE;
                    }
                }
                else
                {
                    /* Started in mode waiting for download request */
                    DebugError0("Started in OS RUN or OS IDLE mode");
                    result = dleds_checkRunModeStatus();
                    if (result == DLEDS_OK)
                    {
                        DLEDS_STATE = WAIT_FOR_REQUEST;
                    }
                    else
                    {
                        /* initialization for DLEDS daemon failed */
                        DebugError0("Function dleds_checkRunModeStatus() returned ERROR");
                    }
                }
                break;
            }
            case WAIT_FOR_REQUEST:
            {
                /*DebugError0("DLEDS ready to receive download request via IPTCom");*/
                if ((result = dleds_wait_for_request()) == DLEDS_OK)
                {
                    DLEDS_STATE = SEND_RESPONSE;
                }
                else if (result == DLEDS_CLEAN_UP_RESET)
                {
                    /* Response message sent on clean up request */
                    /* Progress request sent and progress response received */
                    /* Session data written to file */
                    /* Restart in download mode and make clean up */
                    DLEDS_STATE = RESET_MODE;
                }
                else if (result == DLEDS_EDSTATUS)
                {
                    /* Response message on request has already been sent */
                    /* Wait for next request */
                    DebugError0("Wait for new Request");
                    DLEDS_STATE = WAIT_FOR_REQUEST;
                }
                break;
            }
            case IDLE_MODE:
            {
                DLEDS_STATE = dledsPlatformHandleIdleMode();
                break;
            }
            case DOWNLOAD_MODE:
            {
                if (dleds_download_mode() == DLEDS_OK)
                {
                    DebugError0("Installation of ED package successful");
                }
                else
                {
                    DebugError0("Installation of ED package failed");
                }
                /* Store AppResultCode */
                sessionData.appResultCode = appResultCode;
                /* Check if REQUEST came from own device */
                /* STATUS message should not be sent in that case */
                if (localRequest == TRUE)
                {
                    /* Store information for local request */
                    DebugError1("AppResultCode= (%d) saved in sessionData file", appResultCode);

                    sessionData.transferInProgress = FALSE;
                    sessionData.localTransfer = TRUE;
                    DebugError1("TransferinProgress= (%d) saved in sessionData File", sessionData.transferInProgress);
                    result = dleds_writeStorageFile(&sessionData);
                    result = dleds_readStorageFile(&sessionData);
                    DLEDS_STATE = RESET_MODE;
                }
                else
                {
                    DLEDS_STATE = SEND_STATUS;
                }
                break;
            }
            case SEND_RESPONSE:
            {
                /* Operation mode is RUN or IDLE mode */
                if ((result = dleds_send_response_message()) == DLEDS_OK)
                {
                    DLEDS_STATE = WAIT_FOR_RESPONSE_RESULT;
                }
                else
                {
                    if (result == DLEDS_NO_OF_RETRIES_EXCEEDED)
                    {
                        /* Failed to resend response message, Wait for new request */
                        DLEDS_STATE = WAIT_FOR_REQUEST;
                        /* clear session information */
                        localRequest = FALSE;
                        localRequestInProgress = FALSE;
                        /* clear session data */
                        if (dleds_storageFileExists() == DLEDS_OK)
                        {
                            /* Delete temporary storage file */
                            if (remove(DLEDS_TEMP_STORAGE_FILE_NAME) != 0)
                            {
                                DebugError1("Failed to delete temporary storage file (%s)",
                                    DLEDS_TEMP_STORAGE_FILE_NAME);
                            }
                        }
                    }
                    else
                    {
                        /* Stay in this state, wait and then make a retry to send */
                        IPTVosTaskDelay(10);
                    }
                }
                break;
            }
            case WAIT_FOR_RESPONSE_RESULT:
            {
                if ((result = dleds_wait_for_result()) == DLEDS_OK)
                {
                    /* Operation mode is RUN or IDLE mode */
                    if (appResultCode == DL_OK)
                    {
                        if ( localRequestInProgress == TRUE )
                        {
                            /* Do not reset to DOWNLOAD mode */
                            /* This is a local request so installation of package has already been done */
                            /* send STATUS message with the result from installation */
                            appResultCode = localRequestResultCode;
                            DLEDS_STATE = SEND_STATUS;
                        }
                        else
                        {
                            DLEDS_STATE = RESET_MODE;
                        }
                    }
                    else
                    {
                        /* Result code with error sent as response */
                        /* Wait for next request */
                        DLEDS_STATE = WAIT_FOR_REQUEST;
                    }
                }
                else
                {
                    if (result == DLEDS_NO_OF_RETRIES_EXCEEDED)
                    {
                        DebugError0("Result missing for STATUS message after 3 retries");
                        /* Continue in RUN mode and wait for new request message */
                        DLEDS_STATE = WAIT_FOR_REQUEST;
                        /* clear session data */
                        localRequest = FALSE;
                        localRequestInProgress = FALSE;
                        if (dleds_storageFileExists() == DLEDS_OK)
                        {
                            /* Delete temporary storage file */
                            if (remove(DLEDS_TEMP_STORAGE_FILE_NAME) != 0)
                            {
                                DebugError1("Failed to delete temporary storage file (%s)",
                                    DLEDS_TEMP_STORAGE_FILE_NAME);
                            }
                        }
                    }
                    else
                    {
                        /* Stay in this state. Wait and then send  RESPONSE message again */
                        IPTVosTaskDelay(10);
                        DLEDS_STATE = SEND_RESPONSE;
                    }
                }
                break;
            }
            case SEND_STATUS:
            {
                /* Operation mode is DOWNLOAD mode */
                if ((result = dleds_send_status_message()) == DLEDS_OK)
                {
                    DLEDS_STATE = WAIT_FOR_STATUS_RESULT;
                }
                else
                {
                    if (result == DLEDS_NO_OF_RETRIES_EXCEEDED)
                    {
                        /* Failed to resend status message */
                        /* Check if this STATUS message was sent in RUN mode */
                        if ( localRequestInProgress == TRUE )
                        {
                            /* Already in RUN mode */
                            /* Wait for a new request */
                            localRequest = FALSE;
                            localRequestInProgress = FALSE;
                            DLEDS_STATE = WAIT_FOR_REQUEST;
                            /* clear session data */
                            if (dleds_storageFileExists() == DLEDS_OK)
                            {
                                /* Delete temporary storage file */
                                if (remove(DLEDS_TEMP_STORAGE_FILE_NAME) != 0)
                                {
                                    DebugError1("Failed to delete temporary storage file (%s)",
                                        DLEDS_TEMP_STORAGE_FILE_NAME);
                                }
                            }
                        }
                        else
                        {
                            /* Reset to RUN mode */
                            DLEDS_STATE = RESET_MODE;
                        }
                    }
                    else
                    {
                        /* Stay in this state, wait and then make a retry to send */
                        IPTVosTaskDelay(10);
                    }
                }
                break;
            }
            case WAIT_FOR_STATUS_RESULT:
            {
                if ((result = dleds_wait_for_result()) == DLEDS_OK)
                {
                    if (localRequestInProgress == TRUE )
                    {
                        /* Already in RUN mode */
                        /* Wait for a new request */
                        localRequest = FALSE;
                        localRequestInProgress = FALSE;
                        DLEDS_STATE = WAIT_FOR_REQUEST;
                        /* clear session data */
                        if (dleds_storageFileExists() == DLEDS_OK)
                        {
                            /* Delete temporary storage file */
                            if (remove(DLEDS_TEMP_STORAGE_FILE_NAME) != 0)
                            {
                                DebugError1("Failed to delete temporary storage file (%s)",
                                    DLEDS_TEMP_STORAGE_FILE_NAME);
                            }
                        }
                    }
                    else
                    {
                        /* Reset to RUN mode */
                        DLEDS_STATE = RESET_MODE;
                    }
                }
                else
                {
                    if (result == DLEDS_NO_OF_RETRIES_EXCEEDED)
                    {
                        /* Failed to resend status message */
                        /* */
                        if ( localRequestInProgress == TRUE )
                        {
                            /* Already in RUN mode */
                            /* Wait for a new request */
                            localRequest = FALSE;
                            localRequestInProgress = FALSE;
                            DLEDS_STATE = WAIT_FOR_REQUEST;
                            /* clear session data */
                            if (dleds_storageFileExists() == DLEDS_OK)
                            {
                                /* Delete temporary storage file */
                                if (remove(DLEDS_TEMP_STORAGE_FILE_NAME) != 0)
                                {
                                    DebugError1("Failed to delete temporary storage file (%s)",
                                        DLEDS_TEMP_STORAGE_FILE_NAME);
                                }
                            }
                        }
                        else
                        {
                            /* Reset to RUN mode */
                            DLEDS_STATE = RESET_MODE;
                        }
                    }
                    else
                    {
                        /* Stay in this state. Wait and then send STATUS message again */
                        IPTVosTaskDelay(10);
                        DLEDS_STATE = SEND_STATUS;
                    }
                }
                break;
            }
            case RESET_MODE:
            {
                operationMode = dledsPlatformGetOperationMode();
                if (operationMode == DLEDS_OSDOWNLOAD_MODE)
                {
                    DebugError0("THIS IS A RESET TO RUN MODE");
                    /* send echo-message to prevent that IPTCom interpret
                       the first message in RUN mode as a re-send of STATUS message */
                    dleds_sendEchoMessage();
                    DLEDS_STATE = INITIALIZED;
                    //IPTVosTaskDelay(2000);
                    dleds_resetToRunMode();
                    DebugError0("Wait for reset to strike");
                    //IPTVosTaskDelay(10000);
                }
                else
                {
                    /* operationMode is RUN or IDLE */
                    DebugError0("THIS IS A RESET TO DOWNLOAD MODE");
                    /* send echo-message to prevent that IPTCom interpret
                       the first message in DOWNLOAD mode as a re-send of DATA message */
                    dleds_sendEchoMessage();
                    DLEDS_STATE = INITIALIZED;
                    dleds_resetToDownloadMode();
                    DebugError0("Wait for reset to strike");
                    //IPTVosTaskDelay(10000);
                }
                break;
            }
            case STARTED:
            {
                DebugError0("DLEDS ready to handle request");
                if(dleds_wait_for_request() == DLEDS_ERROR)
                {
                }
                break;
            }
            case STOP:
            {
                DebugError0("DLEDS uninit called");

                IPTVosTaskDelay(1000); /*wait a second for dummy msg post release queue.(otherwise queue will not be destroyed*/

                if(dleds_uninit() != DLEDS_OK)
                {
                    DebugError0("DLEDS uninit terminated with errors");
                }
                else
                {
                    DebugError0("DLEDS uninit terminated successfully");
                }
                DLEDS_STATE = TERMINATED;
                break;
            }
            case TERMINATED:
            {
                done = TRUE;
                break;
            }
            default:
            {
                DebugError0("DLEDS daemon :: Invalid state,  re-setting state to undefined\n");
                DLEDS_STATE = UNDEFINED;
                break;
            }
        }
    }
}
