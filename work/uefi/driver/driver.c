#if 0
#include <Uefi.h>
#include <Protocol/DriverBinding.h>
#include <Protocol/ComponentName2.h>
#include <Protocol/ComponentName.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#endif

#define IN
#define EFIAPI
#define EFI_SUCCESS 0
#define TESTDRIVER_VERSION 0x10
#define OPTIONAL

typedef unsigned short CHAR16;
typedef unsigned char UINT8;
typedef unsigned long long EFI_STATUS, UINTN, UINT32;
typedef void *EFI_HANDLE;

struct _EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL;
typedef
EFI_STATUS
(EFIAPI *EFI_TEXT_STRING) (
    IN struct _EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL  *This,
    IN CHAR16                                   *String
    );

typedef struct _EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL {
    void             *a;
    EFI_TEXT_STRING  OutputString;
} EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL;

typedef struct {
    char                             a[52];
    EFI_HANDLE                       ConsoleOutHandle;
    EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL  *ConOut;
} EFI_SYSTEM_TABLE;

typedef struct {
  UINT8 Type;       ///< 0x01 Hardware Device Path.
  UINT8 SubType;    ///< Varies by Type
  UINT8 Length[2];  ///< Specific Device Path data. Type and Sub-Type define
} EFI_DEVICE_PATH_PROTOCOL;
typedef EFI_DEVICE_PATH_PROTOCOL  EFI_DEVICE_PATH, EFI_DEVICE_PATH_PROTOCOL;

typedef struct _EFI_DRIVER_BINDING_PROTOCOL  EFI_DRIVER_BINDING_PROTOCOL;
typedef
EFI_STATUS
(EFIAPI *EFI_DRIVER_BINDING_SUPPORTED)(
  IN EFI_DRIVER_BINDING_PROTOCOL            *This,
  IN EFI_HANDLE                             ControllerHandle,
  IN EFI_DEVICE_PATH_PROTOCOL               *RemainingDevicePath OPTIONAL
  );

typedef
EFI_STATUS
(EFIAPI *EFI_DRIVER_BINDING_START)(
  IN EFI_DRIVER_BINDING_PROTOCOL            *This,
  IN EFI_HANDLE                             ControllerHandle,
  IN EFI_DEVICE_PATH_PROTOCOL               *RemainingDevicePath OPTIONAL
  );

typedef
EFI_STATUS
(EFIAPI *EFI_DRIVER_BINDING_STOP)(
  IN EFI_DRIVER_BINDING_PROTOCOL            *This,
  IN  EFI_HANDLE                            ControllerHandle,
  IN  UINTN                                 NumberOfChildren,
  IN  EFI_HANDLE                            *ChildHandleBuffer OPTIONAL
  );

struct _EFI_DRIVER_BINDING_PROTOCOL {
  EFI_DRIVER_BINDING_SUPPORTED  Supported;
  EFI_DRIVER_BINDING_START      Start;
  EFI_DRIVER_BINDING_STOP       Stop;
  UINT32                        Version;
  EFI_HANDLE                    ImageHandle;
  EFI_HANDLE                    DriverBindingHandle;
};

// Global
EFI_SYSTEM_TABLE *MySystemTable;

EFI_STATUS
EFIAPI
TestDriverStart (
  IN EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN EFI_HANDLE                   Controller,
  IN EFI_DEVICE_PATH_PROTOCOL     *RemainingDevicePath
  )
{
 
  EFI_STATUS Status;
  Status = EFI_SUCCESS;
  //Print(L"TestDriver , Hello World Inside TestDriverStart .\n");
  MySystemTable->ConOut->OutputString(MySystemTable->ConOut, L"TestDriverStart\n");  
  return Status;
}

EFI_STATUS
EFIAPI
TestDriverSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN EFI_HANDLE                   Controller,
  IN EFI_DEVICE_PATH_PROTOCOL     *RemainingDevicePath
  )
{
  EFI_STATUS                Status;
  Status = EFI_SUCCESS;
  //Print(L"TestDriver , Inside TestDriverSupported .\n");
  MySystemTable->ConOut->OutputString(MySystemTable->ConOut, L"TestDriverSupported\n");  
  return Status;
}

EFI_STATUS
EFIAPI
TestDriverStop (
  IN  EFI_DRIVER_BINDING_PROTOCOL     *This,
  IN  EFI_HANDLE                      Controller,
  IN  UINTN                           NumberOfChildren,
  IN  EFI_HANDLE                      *ChildHandleBuffer
  )
{
  EFI_STATUS                Status;
  Status = EFI_SUCCESS;
  //Print(L"TestDriver , Inside TestDriverStopped .\n");
  MySystemTable->ConOut->OutputString(MySystemTable->ConOut, L"TestDriverStop\n");  
  return Status;
}

#if 0
EFI_DRIVER_BINDING_PROTOCOL gTestDriverBinding = {
  TestDriverSupported,
  TestDriverStart,
  TestDriverStop,
  TESTDRIVER_VERSION,
  NULL,
  NULL
};
#endif

EFI_STATUS
EFIAPI
TestDriverEntryPoint (
  IN EFI_HANDLE                      ImageHandle,
  IN EFI_SYSTEM_TABLE                *SystemTable
  )
{
  EFI_STATUS  Status;
  Status = EFI_SUCCESS;
  MySystemTable = SystemTable;
  MySystemTable->ConOut->OutputString(MySystemTable->ConOut, L"TestDriverEntryPoint\n");  
  //
  // Entry Point
  // Install driver model protocol(s) onto ImageHandle
  //
#if 0
  Status = EfiLibInstallDriverBindingComponentName2 (
             ImageHandle,            // ImageHandle
             SystemTable,            // SystemTable
             &gTestDriverBinding,     // DriverBinding
             ImageHandle,            // DriverBindingHandle
             NULL,
             NULL
             );
  ASSERT_EFI_ERROR (Status);
#endif
  return Status;
}

