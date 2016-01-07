#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <pcap/pcap.h>


char errbuff[PCAP_ERRBUF_SIZE];


typedef struct grep
{
	const char	*pattern;
	char		*filename;
	unsigned	count;
} grep_t;


static void usage(const char * progname)
{
	fprintf(stderr, "usage: %s <pattern> <pcap1 ... pcapN>\n", progname);
}


static void pcap_callback(u_char * user, const struct pcap_pkthdr * hdr, const u_char * bytes)
{
	grep_t	* grep = (grep_t *) user;

	grep->count++;

	if ( memmem(bytes, hdr->caplen, grep->pattern, strlen(grep->pattern)) != NULL )
	{
		printf("%s (pkt %u): found %s\n", grep->filename, grep->count, grep->pattern);
	}
}


int main(int argc, char * argv[])
{
	int		i;
	pcap_t		*pcap;
	grep_t		grep;
	const char	*pattern;

	if ( argc < 3 )
	{
		usage(argv[0]);
		return -1;
	}

	pattern = argv[1];

	/* Iterate over the files
	 */
	for ( i = 2 ; i < argc ; i++ )
	{
		grep.pattern = pattern;
		grep.filename = argv[i];
		grep.count = 0;

		/* Open it
		 */
		if ( (pcap = pcap_open_offline(grep.filename, errbuff)) == NULL )
		{
			fprintf(stderr, "Skip %s: %s\n", grep.filename, errbuff);
			continue;
		}

		/* Iterate over the packets
		 */
		pcap_loop(pcap, 0, pcap_callback, (u_char *)  &grep);

		/* Close it
		 */
		pcap_close(pcap);
	}

	return 0;
}
