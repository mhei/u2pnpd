u²pnpd
======

u²pnpd (or u2pnpd) is a micro UPnP tool with the only purpose to announce the device
on the network via UPnP. So it becomes visible on Windows(TM) workstations and the user
can easily double-click on the device's icon to open the device's web frontend in the
browser.

This is useful for devices which come with factory defaults set to DHCP and thus the
user would need to check with his/her router to obtain the actual device's IP address.

This daemon is designed to run on OpenWrt, so it has some specifics built-in.
However, all defaults detected this way can be overriden by command line arguments.

u²pnpd is release under GPL v2-only.


Reference
---------

http://upnp.org/specs/basic/UPnP-basic-Basic-v1-Device.pdf


Installation
------------

The shell commands are ``./autogen.sh; ./configure; make; make install``.


Report a Bug
------------

To report a bug, you can:
 * fill a bug report on the issue tracker
   http://github.com/mhei/u2pnpd/issues
 * or send an email to mhei@heimpold.de


Acknowledges
------------

This daemon was heavily inspired by UPnP beacon from DigitalStrom project, see
https://gitorious.digitalstrom.org/dss-misc/upnpbeacon


Legal Notes
-----------

Trade names, trademarks and registered trademarks are used without special
marking throughout the source code and/or documentation. All are the property
of their respective owners.
