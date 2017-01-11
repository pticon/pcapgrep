#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <pcap/pcap.h>


#include "grep.h"


#define PROGNAME	"pcapgrep"
#define VERSION		"1.1-dev"


struct pcapgrep
{
	struct grep	*grep;
	const char	*filename;
	unsigned	pkt;
};


static void usage(void)
{
	fprintf(stderr, "usage: %s [options] <pattern> <pcap1 ... pcapN>\n", PROGNAME);
	fprintf(stderr, "options:\n");
	fprintf(stderr, "\t-h           : display this and exit\n");
	fprintf(stderr, "\t-v           : display version number and exit\n");
	fprintf(stderr, "\t-e <pattern> : use <pattern> as the pattern (can be used multiple times)\n");
}


static void version(void)
{
	fprintf(stderr, "%s version %s\n", PROGNAME, VERSION);
}


static void pcap_callback(u_char * user, const struct pcap_pkthdr * hdr, const u_char * bytes)
{
	struct pcapgrep	*pcapgrep = (struct pcapgrep *) user;
	const char	*match;

	pcapgrep->pkt++;

	if ( (match = grep_match(pcapgrep->grep, bytes, hdr->caplen)) )
	{
		printf("%s (pkt %u): found %s\n", pcapgrep->filename, pcapgrep->pkt,
			match);
	}
}


int main(int argc, char * argv[])
{
	int		ret = 0,
			i,
			c;
	pcap_t		*pcap;
	struct pcapgrep	pcapgrep;
	int		has;
	char		errbuff[PCAP_ERRBUF_SIZE];

	if ( (pcapgrep.grep = grep_new()) == NULL )
	{
		fprintf(stderr, "Internal error\n");
		return -1;
	}

	while ( (c = getopt(argc, argv, "hve:")) != -1 )
	{
		switch ( c )
		{
			case 'h':
			usage();
			goto error;

			case 'v':
			version();
			goto error;

			case 'e':
			if ( grep_add_pattern(pcapgrep.grep, optarg) )
			{
				fprintf(stderr, "Internal error\n");
				ret = -1;
				goto error;
			}
			break;
		}
	}

	argc -= optind;
	argv += optind;

	if ( ((has = grep_has_pattern(pcapgrep.grep)) && argc < 1) ||
		(!has && argc < 2) )
	{
		usage();
		ret = -1;
		goto error;
	}

	if ( argc > 1 && grep_add_pattern(pcapgrep.grep, argv[0]) )
	{
		fprintf(stderr, "Internal error\n");
		ret = -1;
		goto error;
	}

	/* Iterate over the files
	 */
	for ( i = argc > 1 ? 1 : 0 ; i < argc ; i++ )
	{
		pcapgrep.filename = argv[i];
		pcapgrep.pkt = 0;

		/* Open the pcap file
		 */
		if ( (pcap = pcap_open_offline(pcapgrep.filename, errbuff)) == NULL )
		{
			fprintf(stderr, "Skip %s: %s\n", pcapgrep.filename, errbuff);
			ret = -1;
			continue;
		}

		/* Iterate over the packets
		 */
		pcap_loop(pcap, 0, pcap_callback, (u_char *)  &pcapgrep);

		/* Close it
		 */
		pcap_close(pcap);
	}

error:
	grep_delete(pcapgrep.grep);

	return ret;
}
