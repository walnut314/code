06-20-2013

The original installation source code for this article has been removed. It
was written to support KMDF 1.0 and no longer functions properly on later
versions of the O/S. Instead of updating the source and letting it rot
again, we instead suggest you use the installation procedure of the 
NONPNP sample in the Windows Driver Kit. This is supported and updated 
as new versions of the framework are created:

http://code.msdn.microsoft.com/windowshardware/NONPNP-5fb0fa62

While it is not a filter driver, it does have the same installation issues
that we have in our class filter driver as described in the article. With
minimal effort, the installation procedure can be modified to install the 
driver. After which it will need to be manually registered as an upper or
lower filter in the appropriate class key.

OSR Team


