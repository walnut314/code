#ifndef _CAMERA_H_
#define _CAMERA_H_

typedef struct _CAM {
    ULONG dude1;
    ULONG dude2;
    ULONG dude3;
    ULONG dude4;

} CAM, *PCAM;

typedef struct _NT_IMAGE_INFO {
   ULONG Version;
   ULONG OsMajorVersion;
   ULONG OsMinorVersion;
   ULONG MajorRelease;
   ULONG LoaderBlockSize;
   ULONG LoaderExtensionSize;
} NT_IMAGE_INFO, *PNT_IMAGE_INFO;

typedef struct _KPROCESS {
   ULONG64 Header;
   LIST_ENTRY ProcessListEntry;
   LIST_ENTRY ProfileListHead;
   ULONG64 DirectoryTableBase;
   LIST_ENTRY ThreadListHead;
   ULONG ProcessLock;
} KPROCESS, *PKPROCESS;

#endif
