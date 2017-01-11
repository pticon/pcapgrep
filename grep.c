#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include "list.h"
#include "grep.h"


struct pattern
{
	char			*exp;
	size_t			len;
	struct list_head	list;
};


struct grep
{
	struct list_head	pattern;
	char			*filename;
	unsigned		count;
};


static struct pattern *pattern_new(void)
{
	struct pattern	*p;

	if ( (p = calloc(1, sizeof(struct pattern))) == NULL )
		return NULL;

	INIT_LIST_HEAD(&p->list);

	return p;
}


static void pattern_delete(struct pattern *p)
{
	if ( p == NULL )
		return;

	free(p->exp);
	free(p);
}


struct grep *grep_new(void)
{
	struct grep	*grep;

	if ( (grep = calloc(1, sizeof(struct grep))) == NULL )
		return NULL;

	INIT_LIST_HEAD(&grep->pattern);

	return grep;
}


void grep_delete(struct grep *grep)
{
	struct list_head	*elt,
				*tmp;
	struct pattern		*p;

	if ( grep == NULL )
		return;

	list_for_each_safe(elt, tmp, &grep->pattern)
	{
		p = list_entry(elt, struct pattern, list);
		list_del(&p->list);
		pattern_delete(p);
	}

	free(grep);
}


int grep_add_pattern(struct grep *grep, const char *pattern)
{
	struct pattern	*p;

	if ( grep == NULL )
		return -1;

	if ( (p = pattern_new()) == NULL )
		return -1;

	if ( (p->exp = strdup(pattern)) == NULL )
		goto error;

	p->len = strlen(p->exp);

	list_add(&p->list, &grep->pattern);

	return 0;

error:
	pattern_delete(p);
	return -1;
}


static int pattern_match(struct pattern *p, const unsigned char *bytes, int len)
{
	return memmem(bytes, len, p->exp, p->len) != NULL;
}


const char *grep_match(struct grep *grep, const unsigned char *bytes, int len)
{
	struct list_head	*elt;
	struct pattern		*p;

	list_for_each(elt, &grep->pattern)
	{
		p = list_entry(elt, struct pattern, list);
		if ( pattern_match(p, bytes, len) )
			return p->exp;
	}

	return NULL;
}


int grep_has_pattern(const struct grep *grep)
{
	if ( grep == NULL )
		return 0;

	return !list_empty(&grep->pattern);
}
