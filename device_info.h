/*
 * Copyright © 2015 Michael Heimpold <mhei@heimpold.de>
 *
 * SPDX-License-Identifier: GPL-2.0
 */

#ifndef DEVICE_INFO_H
#define DEVICE_INFO_H

int deviceinfo_from_cpuinfo(char **dest, const char *key);
int deviceinfo_from_deviceinfo(char **dest, const char *key);
int deviceinfo_from_sysinfo(char **dest, const char *key);

#endif /* DEVICE_MANUFACTURER */
