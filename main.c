// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright © 2015-2017 Michael Heimpold <mhei@heimpold.de>
 *
 */

#include "config.h"
#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <upnp.h>

#include "options.h"
#include "device_info.h"

#define UPNP_ALIVE_INTERVAL 300

#define FN_UUID "/proc/sys/kernel/random/uuid"

struct device_desc {
	char *friendlyName;
	char *manufacturer;
	char *manufacturerURL;
	char *modelDescription;
	char *modelName;
	char *modelNumber;
	char *modelURL;
	char *serialNumber;
	char *uuid;
	char *presentationURL;
	int use_https;
};

int shutdown_flag;

void signal_handler(int signum)
{
	if ((signum == SIGINT) || (signum == SIGTERM))
		shutdown_flag++;

	return;
}

int upnp_callback(Upnp_EventType eventtype, void *event, void *cookie)
{
	return UPNP_E_SUCCESS;
}

int generate_uuid(char **uuid)
{
	FILE *f;
	char *line = NULL;
	size_t len = 0;
	ssize_t c;

	f = fopen(FN_UUID, "rb");
	if (f == NULL)
		return -1;

	if ((c = getline(&line, &len, f)) == -1) {
		fclose(f);
		return -1;
	}

	/* truncate every possible newlines after the uuid */
	if (c > 36)
		line[36] = '\0';

	*uuid = line;
	fclose(f);

	return 36;
}

int quote_xml_special(char **dst, char *src)
{
	char *p, *s;
	int len;

	len = strlen(src);

	/* first source walk-through to determine the required new string len:
	 * each special char needs to be replaced with some additional chars, see
	 * below
	 */
	s = src;
	while (s && *s)
	{
		switch (*s++) {
		case '\'': len += 5; break;
		case '"':  len += 5; break;
		case '<':  len += 3; break;
		case '>':  len += 3; break;
		case '&':  len += 4; break;
		}
	}
	len++; /* NUL-termination */

	p = malloc(len);
	if (!p)
		return -1;

	/* save p now as its changed below */
	*dst = p;

	/* second source walk-through with actual copying data */
	s = src;
	while (s && *s)
	{
		switch (*s) {
		case '\'': strcpy(p, "&apos;"); p += 6; break;
		case '"':  strcpy(p, "&quot;"); p += 6; break;
		case '<':  strcpy(p, "&lt;");   p += 4; break;
		case '>':  strcpy(p, "&gt;");   p += 4; break;
		case '&':  strcpy(p, "&amp;");  p += 5; break;
		default:
			*p++ = *s;
		}

		++s;
	}

	*p = '\0';
	return p - *dst;
}

struct device_desc *device_desc_new(config_options_t *options)
{
	struct device_desc *d = calloc(1, sizeof(*d));
	char *s = NULL;

	if (!d)
		return NULL;

	if (options->uuid)
		quote_xml_special(&d->uuid, options->uuid);
	else
		generate_uuid(&d->uuid);

	if (options->serialNumber) {
		quote_xml_special(&d->serialNumber, options->serialNumber);
	} else {
		deviceinfo_from_cpuinfo(&s, "Serial");
		if (s) {
			quote_xml_special(&d->serialNumber, s);
			free(s);
			s = NULL;
		}
	}

	if (options->modelURL)
		quote_xml_special(&d->modelURL, options->modelURL);
	if (options->modelNumber)
		quote_xml_special(&d->modelNumber, options->modelNumber);
	if (options->modelDescription)
		quote_xml_special(&d->modelDescription, options->modelDescription);

	if (options->manufacturerURL) {
		quote_xml_special(&d->manufacturerURL, options->manufacturerURL);
	} else {
		deviceinfo_from_deviceinfo(&s, "DEVICE_MANUFACTURER_URL");
		if (s) {
			quote_xml_special(&d->manufacturerURL, s);
			free(s);
			s = NULL;
		}
	}

	if (options->manufacturer) {
		quote_xml_special(&d->manufacturer, options->manufacturer);
	} else {
		deviceinfo_from_deviceinfo(&s, "DEVICE_MANUFACTURER");
		if (s) {
			quote_xml_special(&d->manufacturer, s);
			free(s);
			s = NULL;
		}
	}

	if (options->modelName) {
		quote_xml_special(&d->modelName, options->modelName);
	} else {
		deviceinfo_from_deviceinfo(&s, "DEVICE_PRODUCT");
		if (s) {
			quote_xml_special(&d->modelName, s);
			free(s);
			s = NULL;
		}
	}

	if (options->friendlyName) {
		quote_xml_special(&d->friendlyName, options->friendlyName);
	} else {
		deviceinfo_from_deviceinfo(&s, "DEVICE_FRIENDLYNAME");
		if (s) {
			quote_xml_special(&d->friendlyName, s);
			free(s);
			s = NULL;
		} else {
			if (d->manufacturer && d->manufacturer[0] &&
			    d->modelName && d->modelName[0]) {

				if (asprintf(&s, "%s %s", d->manufacturer, d->modelName) != -1) {
					quote_xml_special(&d->friendlyName, s);
					free(s);
					s = NULL;
				}
			}
		}
	}

	if (options->presentationURL) {
		quote_xml_special(&d->presentationURL, options->presentationURL);
	} else {
		if (asprintf(&d->presentationURL, "http%s://%s/", options->use_https ? "s" : "",
		             UpnpGetServerIpAddress()) == -1) {
			/* this should usually not happen */
			d->presentationURL = NULL;
		}
	}

	return d;
}

void device_desc_free(struct device_desc *d)
{
	if (!d)
		return;

	if (d->friendlyName)
		free(d->friendlyName);
	if (d->manufacturer)
		free(d->manufacturer);
	if (d->manufacturerURL)
		free(d->manufacturerURL);
	if (d->modelDescription)
		free(d->modelDescription);
	if (d->modelName)
		free(d->modelName);
	if (d->modelNumber)
		free(d->modelNumber);
	if (d->modelURL)
		free(d->modelURL);
	if (d->serialNumber)
		free(d->serialNumber);
	if (d->uuid)
		free(d->uuid);
	if (d->presentationURL)
		free(d->presentationURL);

	free(d);
}

char *generate_device_desc(config_options_t *options)
{
	struct device_desc *d;
	char *device_desc = NULL;
	int c;

	d = device_desc_new(options);
	if (!d)
		return NULL;

#define CONDITIONAL_PARAM(x) (d->x) ? "<" #x ">" : "", (d->x) ? (d->x) : "", (d->x) ? "</" #x ">" : ""

	c = asprintf(&device_desc,
	    "<?xml version=\"1.0\"?>"
	    "<root xmlns=\"urn:schemas-upnp-org:device-1-0\">"
	         "<specVersion><major>1</major><minor>0</minor></specVersion>"
	         "<device>"
	             "<deviceType>urn:schemas-upnp-org:device:Basic:1</deviceType>"
	             "<UDN>uuid:%s</UDN>"
	             "%s%s%s"
	             "%s%s%s"
	             "%s%s%s"
	             "%s%s%s"
	             "%s%s%s"
	             "%s%s%s"
	             "%s%s%s"
	             "%s%s%s"
	             "%s%s%s"
	         "</device>"
	    "</root>",
	    d->uuid,
	    CONDITIONAL_PARAM(friendlyName),
	    CONDITIONAL_PARAM(manufacturer),
	    CONDITIONAL_PARAM(manufacturerURL),
	    CONDITIONAL_PARAM(modelDescription),
	    CONDITIONAL_PARAM(modelName),
	    CONDITIONAL_PARAM(modelNumber),
	    CONDITIONAL_PARAM(modelURL),
	    CONDITIONAL_PARAM(serialNumber),
	    CONDITIONAL_PARAM(presentationURL));

	free(d);

	return (c != -1) ? device_desc : NULL;
}

int main(int argc, char *argv[])
{
	UpnpDevice_Handle upnp_device;
	char *device_desc = NULL;
	int rv = EXIT_FAILURE;
	struct sigaction sa;

	/* init options, preset some defaults and parse command line arguments */
	options = options_init();
	options_parse_cli(argc, argv, options);

	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = signal_handler;
	sa.sa_flags = 0;
	sigfillset(&sa.sa_mask);

	if (sigaction(SIGINT, &sa, NULL) < 0) {
		perror("sigaction");
		return EXIT_FAILURE;
	}
	if (sigaction(SIGTERM, &sa, NULL) < 0) {
		perror("sigaction");
		return EXIT_FAILURE;
	}

#ifdef UPNP_ENABLE_IPV6
	rv = UpnpInit2(options->interface, 0);
#else
	if (options->interface)
		fprintf(stderr, "Warning: ignoring interface argument, not supported by libupnp.\n");
	rv = UpnpInit(NULL, 0);
#endif
	if (rv != UPNP_E_SUCCESS) {
		fprintf(stderr, "UpnpInit failed: %d\n", rv);
		return EXIT_FAILURE;
	}

	rv = UpnpEnableWebserver(1);
	if (rv != UPNP_E_SUCCESS) {
		fprintf(stderr, "Could not enabled UPnP's internal HTTP server.\n");
		goto upnp_finish;
	}

	device_desc = generate_device_desc(options);
	if (!device_desc) {
		fprintf(stderr, "Could not generated the UPnP device description.\n");
		goto upnp_finish;
	}

	rv = UpnpRegisterRootDevice2(UPNPREG_BUF_DESC, device_desc, strlen(device_desc), 1,
	                             upnp_callback, &upnp_device, &upnp_device);
	if (rv != UPNP_E_SUCCESS) {
		fprintf(stderr, "Failed to register UPnP root device.\n");
		goto free_out;
	}

	rv = UpnpSendAdvertisement(upnp_device, UPNP_ALIVE_INTERVAL);
	if (rv != UPNP_E_SUCCESS) {
		fprintf(stderr, "Failed to announce UPnP device.\n");
		goto upnp_unregister;
	}

	while (!shutdown_flag)
		pause();

	rv = EXIT_SUCCESS;

upnp_unregister:
	UpnpUnRegisterRootDevice(upnp_device);
free_out:
	if (device_desc)
		free(device_desc);
upnp_finish:
	UpnpFinish();
	options_free(options);
	return rv;
}
