WDFFLTR V1.0

http://www.osronline.com/article.cfm%5earticle=446.htm

This archive contains a generic KMDF class filter driver and two associated applications,
one to install the filter and one to test user-driver communication.

The directory structure of the archive is as follows:

\app

Contains source code for a user mode application that logs events coming
from the filter driver. It is provided only as a skeleton to show how
to interact with a WDF filter driver via a control device. The solution 
provided is not meant to be complete or ideal, just straightforward.

\filter

The source code for the generic WDF filter driver. Please read through the large 
comment blocks at the beginning of the wdffltr.cpp module and in the WdfFltrQueryRemove
callback as they provide important cautionary information regarding a possible
BSOD on unload due to an issue in KMDF 1.0. 


\inc

Common includes between the application and the filter driver


\install

Command line installation utility. This utility will go through all of the 
required steps to install the filter driver itself, while making the appropriate
calls to the WDF coinstaller (which much reside in the same directory as 
the application). Important notes about the installer:

1) An INF file IS NOT REQUIRED and will be IGNORED by the installer. 
When installing a class filter driver only the stub of an INF file is
required, and the installer will autogenerate that stub.

2) The installer does NOT register the filter driver service as an
upper or lower filter to any class of device. The user must manually
(or programmatically) add the appropriate UpperFilters/LoweFilters key
his/herself.
 

++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


All software and documentation supplied in this archive is:

Copyright © 2004-2006 OSR Open Systems Resources, Inc.

This software is a licensed software product, provided for instructional purposes only, and subject to the terms and conditions indicated below. If you do not agree to comply with the terms and conditions indicated, do not use this software and delete all copies in your possession.

 

Warranty

OSR Open Systems Resources, Inc. (OSR) expressly disclaims any warranty for this software. THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF MECHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE. THE ENTIRE RISK ARISING FROM THE USE OF THIS SOFTWARE REMAINS WITH YOU. OSR's entire liability and your exclusive remedy shall not exceed the price paid for this material. In no event shall OSR or its suppliers be liable for any damages whatsoever (including, without limitation, damages for loss of business profit, business interruption, loss of business information, or any other pecuniary loss) arising out of the use or inability to use this software, even if OSR has been advised of the possibility of such damages. Because some states/jurisdictions do not allow the exclusion or limitation of liability for consequential or incidental damages, the above limitation may not apply to you.

 

License: Making Copies or Using This Software

You may make copies or derivatives of the software and documentation contained herein for educational purposes, provided OSR’s copyrights remain present as they are provided herein. You may make copies or derivatives of the software and documentation contained herein for commercial purposes, provided that (a) OSR’s copyrights remain present as they are provided herein, (b) that you provide significant value added beyond that provided by the software and documentation provided by OSR, and (c) that the following text appears

            Based in part on software Copyright 2004-2006 © OSR Open Systems Resources, Inc.
            Used with permission

in each location and in identical text and type size that your copyright appears for the commercial use containing this work.

 
