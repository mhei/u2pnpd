u²pnpd
======

[![Build Status](https://travis-ci.org/mhei/u2pnpd.svg?branch=master)](https://travis-ci.org/mhei/u2pnpd)

u²pnpd (or u2pnpd) is a micro UPnP tool with the only purpose to announce the device
on the network via UPnP. So it becomes visible on Windows(TM) workstations and the user
can easily double-click on the device's icon to open the device's web frontend in the
browser.

This is useful for devices which come with factory defaults set to DHCP and thus the
user would need to check with his/her router to obtain the actual device's IP address.

This daemon is designed to run on OpenWrt, so it has some specifics built-in.
However, all defaults detected this way can be overriden by command line arguments.
These argument's names match the corresponding XML tags in the UPnP spec, so it
should be easy to understand their meaning. If you don't specify one, the tag is
omitted in the device descriptor.
A special note to the UUID: the UUID should be the same between various invocations.
This way a UPnP client can detect changes of e.g. the device's IP address and
discard stale (cached) data. So it's best to create a UUID once, and then pass this
to u²pnpd via parameter. When this parameter is not passed, then a random UUID is
obtained from the kernel (/proc/sys/kernel/random/uuid).

Yes, this isn't really a daemon (yet) - use something like start-stop-daemon to bring
this into background. Or in other words: this is left as an exercise to the reader :-)

u²pnpd is released under GPL v2-only.


Reference
---------

http://upnp.org/specs/basic/UPnP-basic-Basic-v1-Device.pdf


Installation
------------

The shell commands are ``./autogen.sh; ./configure; make; make install``.


Usage
-----

```
./u2pnpd (u2pnpd 0.5) -- tool to announce this device via UPnP on the network

Usage: ./u2pnpd [<options>]

Options:
        -f, --friendlyName      a short user-friendly title (default: [manufacturer] [modelName])
        -M, --manufacturer      manufacturer name
        -m, --manufacturerURL   URL to manufacturer site
        -D, --modelDescription  long user-friendly title
        -P, --modelName         model name
        -N, --modelNumber       model number
        -p, --modelURL          URL to manufacturer's product pages
        -S, --serialNumber      manufacturer's serial number
        -u, --uuid              UUID of this device
        -s, --httpsURL          generate a HTTPS URL for the presentation URL (HTTP is used by default)
        -i, --interface         network interface to use
        -V, --version           print version and exit
        -h, --help              print this usage and exit
```


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
