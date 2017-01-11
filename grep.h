#ifndef __GREP_H__
#define __GREP_H__


struct grep;


struct grep *grep_new(void);

void grep_delete(struct grep *grep);

int grep_add_pattern(struct grep *grep, const char *pattern);

const char *grep_match(struct grep *grep, const unsigned char *bytes, int len);

int grep_has_pattern(const struct grep *grep);

#endif /* __GREP_H__ */
