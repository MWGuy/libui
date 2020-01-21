// 19 january 2020
#include "test.h"

struct test {
	const char *name;
	int (*f)(void);
};

static struct test *tests = NULL;
static size_t lenTests = 0;
static size_t capTests = 0;

void testingprivRegisterTest(const char *name, int (*f)(void))
{
	if (lenTests == capTests) {
		struct test *newtests;

		capTests += 32;
		newtests = (struct test *) realloc(tests, capTests * sizeof (struct test));
		if (newtests == NULL) {
			fprintf(stderr, "memory exhausted registering test %s\n", name);
			exit(1);
		}
		tests = newtests;
	}
	tests[lenTests].name = name;
	tests[lenTests].f = f;
	lenTests++;
}

static int testcmp(const void *aa, const void *bb)
{
	const struct test *a = (const struct test *) aa;
	const struct test *b = (const struct test *) bb;

	return strcmp(a->name, b->name);
}

static const char *basename(const char *file)
{
	const char *p;

	for (;;) {
		p = strpbrk(file, "/\\");
		if (p == NULL)
			break;
		file = p + 1;
	}
	return file;
}

void testingprivLogf(FILE *f, const char *filename, long line, const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	fprintf(f, "%s:%ld: ", basename(filename), line);
	vfprintf(f, fmt, ap);
	fprintf(f, "\n");
	va_end(ap);
}

int main(int argc, char *argv[])
{
	struct test *t;
	struct test want;

	if (argc != 2) {
		fprintf(stderr, "usage: %s TestName\n", argv[0]);
		return 1;
	}
	qsort(tests, lenTests, sizeof (struct test), testcmp);
	want.name = argv[1];
	t = (struct test *) bsearch(&want, tests, lenTests, sizeof (struct test), testcmp);
	if (t == NULL) {
		fprintf(stderr, "%s: no such test\n", argv[1]);
		return 1;
	}
	return (*(t->f))();
}
