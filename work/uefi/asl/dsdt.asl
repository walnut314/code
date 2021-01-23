
// iasl dsdt.asl
// acpiexec dsdt.aml
// evaluate OBJ1 
// execute CTDW 7
// execute XOSC

DefinitionBlock ("", "DSDT", 2,"","",0x0)
{

    Method (MAIN, 0, NotSerialized)
    {
        Store (__FILE__, Debug)
        Store (__LINE__, Debug)
        Store (__DATE__, Debug)
    }

    Method (CTDW, 1)
    {
        local0 = arg0
        while (local0)
        {
            printf ("%o", local0)
            local0--;
        }
    }
    Method (CTUP, 1)
    {
        local0 = arg0
        while (local0 < 10)
        {
            if (!(local0 % 2))
            {
                printf ("%o is even", local0)
            }
            else
            {
                printf ("%o is odd", local0)
            }
            local0++;
        }
    }

    // strings
    Name (STR1, "it is a wonderful day")
    Method (DBG1)
    {
        printf ("Hello world, %o", STR1)
        printf ("uuid %o", ToUUID("33DB4D5B-1FF7-401C-9657-7441C03DD766"))
    }
    Method (DBG2)
    {
        local0 = Concatenate ("Hello world, ", STR1)
        printf ("%o", local0)
    }    

    // buffers - buffer elements need to be dereferenced
    //           before being accessed - via Derefof
    // call this with a string - "wusup dawg" prints hex
    Method (IDXA, 1)
    {
        local0 = sizeof(arg0)
        local1 = 0 // use this as the index value
        printf ("The size of the buffer is %o", local0)
        while (local0 > local1)
        {
            printf ("%o", Derefof(arg0[local1]))
            local1++
        }
    }    

    // Packages are similar to buffers except their elements can contain strings, 
    // integers, buffers, and other named objects. The above method works for 
    // both buffers and packages.
    Method (OVFL, 1) // Causes a buffer overrun
    {
        local0 = sizeof(arg0)
        local1 = 0 // use this as the index value
        printf ("The size of the buffer is %o", local0)
        while (local0 >= local1) // remove the = to not overflow
        {
            printf ("%o", derefof(arg0[local1])) //runtime error
            local1++
        }
        printf ("Complete")
    }

    // 5.5 Bit fields
    /*
        CreateBitField      Length: 1 bit
        CreateByteField     Length: 1 byte
        CreateWordField     Length: 2 bytes
        CreateDWordField    Length: 4 bytes
        CreateQWordField    Length: 8 bytes
        CreateField         Length: Arbitrary
    */
    Name(SUPP,0) // PCI _OSC Support Field value
    Name(CTRL,0) // PCI _OSC Control Field value
    /*
        _OSC
        Arg0 A Buffer containing a UUID
        Arg1 An Integer containing a Revision ID of the buffer format
        Arg2 An Integer containing a count of entries in Arg3
        Arg3 A Buffer containing a list of DWORD capabilities
    */
    Method(_OSC, 4) {
        printf ("OSC uuid %o", Arg0)
        printf ("OSC rev  %o", Arg1)
        printf ("OSC cnt  %o", Arg2)
        printf ("OSC res  %o", Arg3)

        CreateByteField(Arg3,  0, CDW1)
        CreateByteField(Arg3,  4, CDW2)
        CreateByteField(Arg3,  8, CDW3)
        CreateByteField(Arg3, 10, CDW4)
        
        printf ("OSC CDW1 %o", CDW1)
        printf ("OSC CDW2 %o", CDW2)
        printf ("OSC CDW3 %o", CDW3)
        printf ("OSC CDW3 %o", CDW4)

        Store(CDW2,SUPP)
        Store(CDW3,CTRL)
        If(LNotEqual(And(SUPP, 0x16), 0x16)) {
            And(CTRL,0x1E,CTRL) // Mask bit 0 (and undefined bits)
        }

        // Always allow native PME, AER (no dependencies)
        // Never allow SHPC (no SHPC controller in this system)
        And(CTRL,0x1D,CTRL)

        If(Not(And(CDW1,1))) { // Query flag clear?
            // Disable GPEs for features granted native control.
            If(And(CTRL,0x01)) { // Hot plug control granted?
                //Store(0,HPCE) // clear the hot plug SCI enable bit
                //Store(1,HPCS) // clear the hot plug SCI status bit
            }
        }
        Return(Arg3)
    }
    Method (XOSC, 0, NotSerialized) {
        _OSC(ToUUID("33DB4D5B-1FF7-401C-9657-7441C03DD766"), 
                0x42,
                12,
                Buffer(8){0x00, 0x01, 0x02, 0x03, 
                          0x04, 0x05, 0x06, 0x07,
                          0x08, 0x09, 0x0a, 0x0b})
    }

    // 5.6 Resource Templates
    Method (_CRS, 0, Serialized)
    {
        Name (RBUF, ResourceTemplate ()
        {
            GpioInt (Edge, ActiveLow, ExclusiveAndWake, PullNone, 0x0000, "\\_SB.GPIO", 0x00, ResourceConsumer, , ) {2}  // PCIE_WAK
            GpioInt (Edge, ActiveHigh, SharedAndWake, PullNone, 0x0000, "\\_SB.GPIO", 0x00, ResourceConsumer, , ) {172}  // NbGppPme
        })
        Return(RBUF)		   //Yes, return GPIO interrupt info
    }

    Name (RES2,
        ResourceTemplate(){
            IO(Decode16, 0x62, 0x62, 0, 1, IOP1)
            IO(Decode16, 0x66, 0x66, 0, 1, IOP2)
            IRQ (Level, ActiveLow, Shared, IRQ1) {0x0C}             
    })
    Method (WRRT)
    {
        /*
         * According to the ACPI specification, the second
         * field of the IO macro is 16 bits. Therefore, use
         * CreateWordField to overlay a region that is 16 bits
         * in length.
         */
        CreateWordField (RES2, \IOP1._MIN, MINF)
        MINF = 0x1234 // This will write 0x1234
        // to the second field in IOP1.
        printf("WRRT IOP1 decode: %o min: %o max: %o aln: %o len: %o", \IOP1._DEC, \IOP1._MIN, \IOP1._MAX, \IOP1._ALN, \IOP1._LEN)
        printf("WRRT IRQ1 %o level: %o active: %o shared: %o", \IRQ1, \IRQ1._HE, \IRQ1._LL, \IRQ1._SHR)
    }
    Method (XWRT) {
        WRRT()
    }

    // 5.7 Control Method Calling Convention (call by value or reference)
    Name (DUDE, 0x00)
    Name (BUFD, Buffer() {0x47})
    Name (PKGD, Package() {DUDE})
    Method (CHG1, 1, NotSerialized) {
        printf("%o", DUDE)
        DUDE = arg0
        printf("%o", DUDE)
        arg0[0] = 0x12
    }
    Method (CHG2) {
        //CHG1(PKGD)
        CHG1(BUFD)
        printf("%o", PKGD)
        printf("%o", BUFD)
    }


    // Notify
    Method(_WAK,1,Serialized) {
        Notify(\DEV1, 0x02) // (1 for S1, 2 for S2, etc.)
        local0 = \DEV1.STA1
        printf ("Device state %o, arg %o", local0, arg0)

        if (local0 == 0x42) {
            return (Package(2) {
                0x00000001,   // Element 0 - Wake was signaled and was successful
                0x00000002    // Element 1 - Power supply S-state unsupported
            })
        } else {
            return (Package(2) {
                0x00000000,   // Element 0 - Wake was signaled and was successful
                0x00000000    // Element 1 - Power supply S-state unsupported
            })
        }
    }

    Name (OBJ0, 0x1234)
    Name (OBJ1, "HELLO WORLD")
    Method (TEST, 2) {
        printf ("Arg0 %o", Arg0)
        printf ("Arg2 %o", Arg1)
    }
    Name (BUF1, Buffer(3){0x00, 0x01, 0x02})
    Name (BUF2, Buffer(){0x00, 0x01, 0x02, 0x03})
    Name (PKG1, Package() {OBJ1, BUF1})

    // used by methods
    // SystemMemory, SystemIO, PCIConfig,... see ACPI spec
    OperationRegion(OPR1, SystemMemory, 0x10000, 0x5)
    Field (OPR1)
    {
        FLD1, 8,
        FLD2, 8,
        Offset (3),
        FLD3, 4,
        FLD4, 12
    }    
    // ASL role: represent devices that are not natively enumerable
    // see ACPI spec 5.6.8 for predefined names
    Device (DEV1)
    {
        Name (_HID, "DEADBEEF")
        Name (_CRS, Buffer() {1,2,3,4}) // buffer containing resource setting
        Name (INT1, 0x1234)
        Name (STA1, 0x42)
        Name (STR1, "This string is inside of DEV1's scope")
        Name (BUF1, Buffer() {0x00, 0x04, 0x6f})
        Name (PKG1, Package() {OBJ0, OBJ1})        
    }

    // USB host controller
    // This device can contain many ports
    Device (USBH)
    {
        Name (_HID, "DEADBEE0")
        Name (_MIN, 0x47)
        // evaluate USBH._CRS            
        Name(_CRS, ResourceTemplate() {
            IO(Decode16, 0x0, 0x0, 0x1, 0x10)
            IO(Decode16, 0x81, 0x81, 0x1, 0xF)
            IO(Decode16, 0xC0, 0xC0, 0x1, 0x20)
            DMA(Compatibility, NotBusMaster, Transfer8_16) {4}
            IRQNoFlags(){0xD}
        })

        Device (USB1) // USB port #1
        {
            Name (_HID, "DEADBEE1")
            Name (INT1, 0x1234)
            Name (STR1, "USB port 1")
            Name (BUF1, Buffer() {0x00, 0x04, 0x6f})
            Name (PKG1, Package() {OBJ0, OBJ1})
        }
        Device (USB2) // USB port #2
        {
            Name (_HID, "DEADBEE2")
            Name (INT1, 0xABCD)
            Name (STR1, "USB port 2")
            Name (BUF1, Buffer() {0x01, 0x05, 0x70})
            Name (PKG1, Package() {OBJ0, OBJ1})
        }
    }

    // Methods -- executable ASL
    // Method (MethodName, NumArgs, SerializeRule)
    Name (INT1, 0x1234)
    Method (MTH1, 1, NotSerialized)
    {
        Local0 = 0xdead0000
        INT1 = Arg0 + local0
        // optional Return XXX
    }

}

