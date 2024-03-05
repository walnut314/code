#ifndef _EPRINTF_H_
#define _EPRINTF_H_


extern void eprintf(char *, ...);
extern void weprintf(char *, ...);
extern char *estrdup(char *);
extern void *emalloc(size_t);
extern void *remalloc(void *, size_t);
extern char *progname(void);
extern void setprognam(char *);

#endif // _EPRINTF_H_
