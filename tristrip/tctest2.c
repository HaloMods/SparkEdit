/*
 * $Header: /home/grantham/cvsroot/projects/modules/tc/tctest2.c,v 1.14 2000/10/03 07:20:56 grantham Exp $
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <signal.h>
#include <unistd.h>
#include "tc.h"

/* SNIPPET "timer.c" Inducted Wed Nov 22 09:36:57 2000 */

#include <sys/types.h>
#include <malloc.h>

#if !defined(MEM_CHART)
#define chartedSetLabel(a)
#endif

#if defined(_POSIX93)
#include <sys/time.h>
#else /* POSIX93 */
#if defined(__linux)
#include <sys/time.h>
#else /* __linux__ */
#include <sys/timeb.h>
#endif /* __linux */
#endif /* POSIX93 */

typedef struct {
#if defined(_POSIX93)
    struct timespec Ts;
#else /* POSIX93 */
#if defined(__linux__)
    struct timeval Ts;
#else /* __linux__ */
    struct _timeb Ts;
#ifdef _WIN32
    LONGLONG PerfCount;
#endif /* _WIN32 */
#endif /* __linux__ */
#endif /* POSIX93 */
} Timer;

#ifdef _WIN32
static LARGE_INTEGER	scratch;
static int		useHighPerf;
static LONGLONG		perfFreq;
#endif /* _WIN32 */

void
timerInit(void)
{
#if defined(_WIN32)
    if(!QueryPerformanceFrequency(&scratch)) {
        useHighPerf = 0;
    } else {
        useHighPerf = 1;
	perfFreq = scratch.QuadPart;
    }
#endif
}

void
timerReset(Timer *t)
{
#if defined(_POSIX93)
    clock_gettime(CLOCK_REALTIME, &t->Ts);
#else /* POSIX93 */
#if defined(__linux__)
    gettimeofday(&t->Ts, NULL);
#else /* __linux__ */
#if defined(_WIN32)
    if(useHighPerf) {
        QueryPerformanceCounter(&scratch);
	t->PerfCount = scratch.QuadPart;
    } else
#endif /* _WIN32 */
        _ftime(&t->Ts);
#endif /* __linux__ */
#endif /* POSIX93 */
}

double
timerGet(Timer *t)
{
#if defined(_POSIX93)
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return ts.tv_sec - t->Ts.tv_sec + (ts.tv_nsec - t->Ts.tv_nsec) / 1.e9;
#else /* POSIX93 */
#if defined(__linux__)
    struct timeval ts;
    gettimeofday(&ts, NULL);
    return ts.tv_sec - t->Ts.tv_sec + (ts.tv_usec - t->Ts.tv_usec) / 1.e6;
#else /* __linux__ */
    struct _timeb ts;
#if defined(_WIN32)
    if(useHighPerf) {
        QueryPerformanceCounter(&scratch);
        return (scratch.QuadPart - t->PerfCount) / (double)perfFreq;
    } else
#endif
    {
        _ftime(&ts);
        return ts.time - t->Ts.time + 0.001 * (ts.millitm - t->Ts.millitm);
    }
#endif /* __linux__ */
#endif /* POSIX93 */
}

Timer *timerNew(void)
{
    Timer *t;

    chartedSetLabel("new timer");
    t = (Timer *)malloc(sizeof(Timer));
    timerReset(t);
    return t;
}


/* "timer.c" ENDSNIPPET */

#if !defined(MEM_CHART)
#define chartedSetLabel(a)
#endif

#define STRING(a) #a

#define CHECKNOT(a, d) \
    { \
	int r; \
	r = (a); \
	if(r != (d)) { \
	    fprintf(stderr, "%d: \"" STRING(a)  "\" should have been " \
	        "%08x, was %s%04X\n", __LINE__, (d), (r < 0) ? "-" : "", \
		(r < 0) ? -r : r); \
	    exit(1); \
	} \
    }

#define CHECKERR(a) \
    { \
	int r; \
	r = (a); \
	if(r < 0) { \
	    fprintf(stderr, "%d: \"" STRING(a) "\" failed with %04X\n", \
	        __LINE__, -r); \
	    exit(1); \
	} \
    }

int triangleCount;
uint (*triangles)[3];
int triangleResultsCount;
uint (*triangleResults)[3];

int (*primTypes);
int (*primLengths);
uint (*primVerts);

int compareTriangle(const void *k1, const void *k2)
{
    uint *t1 = (int *)k1;
    uint *t2 = (int *)k2;

    if(t2[0] != t2[0]) {
        return t1[0] - t2[0];
    }
    if(t1[1] != t2[1]) {
        return t1[1] - t2[1];
    }
    return t1[2] - t2[2];
}

int findTriangle(uint v1, uint v2, uint v3)
{
    uint t[3];

    t[0] = v1; t[1] = v2; t[2] = v3;
    if(bsearch(t, triangles, triangleCount, sizeof(int) * 3, compareTriangle)
        != NULL)
	return 1;

    t[0] = v2; t[1] = v3; t[2] = v1;
    if(bsearch(t, triangles, triangleCount, sizeof(int) * 3, compareTriangle)
        != NULL)
	return 1;

    t[0] = v3; t[1] = v1; t[2] = v2;
    if(bsearch(t, triangles, triangleCount, sizeof(int) * 3, compareTriangle)
        != NULL)
	return 1;

    return 0;
}

void dumpTriangles(FILE *f, int count, uint (*triangles)[3])
{
    int i;

    fprintf(f, "%d triangles input:\n", count);
    for(i = 0; i < count; i++)
	fprintf(f, "   %u %u %u\n", triangles[i][0], triangles[i][1],
	    triangles[i][2]);
}

void abortme(int sig)
{
    FILE *f = fopen("failure.out", "a");
    if(f == NULL)
        f = stderr;
    signal(SIGABRT, SIG_DFL);
    fprintf(f, "original triangles:\n");
    dumpTriangles(f, triangleCount, triangles);
    fprintf(f, "what I had so far:\n");
    dumpTriangles(f, triangleResultsCount, triangleResults);
    fclose(f);
    abort();
}

int main(int argc, char **argv)
{
    ACTCData *tc;
    uint minVert;
    uint vertRange;
    int i, j, k, n;
    uint v1, v2, v3;
    int prim;
    int swap;
    int vertices;
    Timer *timer;
    int argsused;
    int reps = 100;
    int isBackwardsOkay = 1;
    int fwdtris;
    int rvstris;
    int useOneStopShop = 0;
    int maxBatchSize = INT_MAX;
    int primCount;
    void *oldBrk;

    timerInit();
    timer = timerNew();

    signal(SIGABRT, abortme);

    tc = actcNew();
    if(tc == NULL) {
	printf("failed to allocate TC structure\n");
	exit(1);
    }

    argc--;
    argv++;
    while(argc > 0) {
	argsused = 1;
	if(strcmp(argv[0], "-h") == 0) {
	    fprintf(stderr, "tctest2 options:\n");
	    fprintf(stderr, "\t-minfanverts #    - set MIN_FAN_VERTS\n");
	    fprintf(stderr, "\t-maxprimverts #   - set MAX_PRIM_VERTS\n");
	    fprintf(stderr, "\t-honorwinding #   - set HONOR_WINDING\n");
	    fprintf(stderr, "\t-reps #           - set test repetitions\n");
	    fprintf(stderr, "\t-seed #           - set random seed\n");
	    fprintf(stderr, "\t-onestop #        - use "
	        "TrianglesToPrimitives\n");
	    fprintf(stderr, "\t-maxbatch #       - limit # of tris that "
	        "TrianglesToPrimitives can feed\n    in during one pass\n");
	    exit(1);
	} else if(strcmp(argv[0], "-onestop") == 0) {
	    useOneStopShop = 1;
	    argsused = 1;
	} else if(strcmp(argv[0], "-maxbatch") == 0) {
	    if(argc < 2) {
		fprintf(stderr, "option -maxbatch requires an argument\n");
		exit(1);
	    }
	    maxBatchSize = atoi(argv[1]);
	    argsused = 2;
	} else if(strcmp(argv[0], "-seed") == 0) {
	    if(argc < 2) {
		fprintf(stderr, "option -seed requires an argument\n");
		exit(1);
	    }
	    srandom(atoi(argv[1]));
	    argsused = 2;
	} else if(strcmp(argv[0], "-minfanverts") == 0) {
	    if(argc < 2) {
		fprintf(stderr, "option -minfanverts requires an argument\n");
		exit(1);
	    }
	    CHECKERR(actcParami(tc, ACTC_OUT_MIN_FAN_VERTS, atoi(argv[1])));
	    argsused = 2;
	} else if(strcmp(argv[0], "-maxprimverts") == 0) {
	    if(argc < 2) {
		fprintf(stderr, "option -maxprimverts requires an argument\n");
		exit(1);
	    }
	    CHECKERR(actcParami(tc, ACTC_OUT_MAX_PRIM_VERTS, atoi(argv[1])));
	    argsused = 2;
	} else if(strcmp(argv[0], "-honorwinding") == 0) {
	    if(argc < 2) {
		fprintf(stderr, "option -honorwinding requires an argument\n");
		exit(1);
	    }
	    isBackwardsOkay = !atoi(argv[1]);
	    CHECKERR(actcParami(tc, ACTC_OUT_HONOR_WINDING, atoi(argv[1])));
	    argsused = 2;
	} else if(strcmp(argv[0], "-reps") == 0) {
	    if(argc < 2) {
		fprintf(stderr, "option -reps requires an argument\n");
		exit(1);
	    }
	    reps = atoi(argv[1]);
	    argsused = 2;
	} else {
	    fprintf(stderr, "didn't understand command line "
	        "parameter \"%s\"\n", argv[0]);
	    exit(1);
	}
	argc -= argsused;
	argv += argsused;
    }

/* Random triangles */

    oldBrk = sbrk(0); /* XXX - This is going to break under Win32 */
    for(i = 0; i < reps; i++) {
	switch(random() % 4) {
	    case 0: minVert = random(); break;
	    case 1: minVert = random() % 100000; break;
	    case 2: minVert = random() % 1000; break;
	    default: case 3: minVert = random() % 10; break;
	}
	if((random() % 2) == 0)
	    minVert += INT_MAX;
	switch(random() % 4) {
	    case 0: vertRange = 3 + random() % (RAND_MAX - minVert); break;
	    case 1: vertRange = 3 + random() % 100000; break;
	    case 2: vertRange = 3 + random() % 1000; break;
	    default: case 3: vertRange = 3 + random() % 10; break;
	}
	if((random() % 2) == 0)
	    vertRange += INT_MAX;
	if(vertRange > UINT_MAX - minVert)
	    vertRange = UINT_MAX - minVert;
	switch(random() % 10) {
	    case 0: triangleCount = 3 + random() % 1000000; break;
	    case 1: triangleCount = 3 + random() % 100000; break;
	    case 2: case 3: triangleCount = 3 + random() % 100; break;
	    default: triangleCount = 3 + random() % 10000; break;
	}
	chartedSetLabel("triangles");
	triangles = realloc(triangles, sizeof(uint) * 3 * triangleCount);
	chartedSetLabel("triangle results");
	triangleResults = realloc(triangleResults, sizeof(uint) * 3 *
	    triangleCount);
	printf("set %d contains %d triangles with vertices ranging %u to %u\n",
	    i, triangleCount, minVert, minVert + vertRange - 1);

	actcParamu(tc, ACTC_IN_MIN_VERT, minVert);
	actcParamu(tc, ACTC_IN_MAX_VERT, minVert + vertRange - 1);

	for(j = 0; j < triangleCount; j++) {
	    triangles[j][0] = minVert + random() % vertRange;
	    triangles[j][1] = minVert + random() % vertRange;
	    triangles[j][2] = minVert + random() % vertRange;
	}

	vertices = 0;
	triangleResultsCount = 0;
	if(useOneStopShop) {
	    timerReset(timer);
	    printf("transforming triangles to primitives... "); fflush(stdout);
	    chartedSetLabel("primlengths");
	    primLengths = realloc(primLengths, sizeof(int) * triangleCount);
	    chartedSetLabel("primtypes");
	    primTypes = realloc(primTypes, sizeof(int) * triangleCount);
	    chartedSetLabel("primverts");
	    primVerts = realloc(primVerts, sizeof(uint) * 3 * triangleCount);
	    primCount = actcTrianglesToPrimitives(tc, triangleCount, triangles,
	        primTypes, primLengths, primVerts, maxBatchSize);
	    if(primCount < 0)
		CHECKERR(primCount);
	    n = 0;
	    for(j = 0; j < primCount; j++) {
		swap = 0;
		v1 = primVerts[n++];
		v2 = primVerts[n++];
		for(k = 2; k < primLengths[j]; k++) {
		    v3 = primVerts[n++];
		    if(primTypes[j] == ACTC_PRIM_STRIP && (swap % 2) == 1){
			triangleResults[triangleResultsCount][0] = v2;
			triangleResults[triangleResultsCount][1] = v1;
			triangleResults[triangleResultsCount][2] = v3;
		    } else {
			triangleResults[triangleResultsCount][0] = v1;
			triangleResults[triangleResultsCount][1] = v2;
			triangleResults[triangleResultsCount][2] = v3;
		    }
		    swap++;
		    if(primTypes[j] == ACTC_PRIM_STRIP) {
			v1 = v2;
		    }
		    v2 = v3;
		    triangleResultsCount++;
		}
	    }
	    printf(" %g tri/sec\n", triangleCount /
		timerGet(timer));
	} else {
	    timerReset(timer);
	    printf("adding triangles... "); fflush(stdout);
	    CHECKERR(actcBeginInput(tc))
	    for(j = 0; j < triangleCount; j++) {
		CHECKERR(actcAddTriangle(tc, triangles[j][0], triangles[j][1],
		    triangles[j][2]));
	    }
	    CHECKERR(actcEndInput(tc));
	    printf(" %g triangles added per second\n", triangleCount /
		timerGet(timer));

	    timerReset(timer);
	    printf("getting primitives... "); fflush(stdout);
	    CHECKERR(actcBeginOutput(tc));
	    while((prim = actcStartNextPrim(tc, &v1, &v2))
	        != ACTC_DATABASE_EMPTY) {
		vertices += 2;
		swap = 0;
#if defined(INFO)
		printf("primitive type %04x\n", prim);
#endif
		while(actcGetNextVert(tc, &v3) != ACTC_PRIM_COMPLETE) {
		    if(prim == ACTC_PRIM_STRIP && (swap % 2) == 1){
			triangleResults[triangleResultsCount][0] = v2;
			triangleResults[triangleResultsCount][1] = v1;
			triangleResults[triangleResultsCount][2] = v3;
		    } else {
			triangleResults[triangleResultsCount][0] = v1;
			triangleResults[triangleResultsCount][1] = v2;
			triangleResults[triangleResultsCount][2] = v3;
		    }
		    swap++;
		    if(prim == ACTC_PRIM_STRIP) {
			v1 = v2;
		    }
		    v2 = v3;
		    triangleResultsCount++;
		    vertices ++;
		}
	    }
	    CHECKERR(actcEndOutput(tc));
	    printf(" %g vertices retrieved per second\n", vertices /
		timerGet(timer));
	}
	if(triangleResultsCount > triangleCount) {
	    printf("TC gave back more triangles than tctest2 put in!\n");
	    abort();
	}
	if(triangleResultsCount < triangleCount) {
	    printf("TC gave back fewer triangles than tctest2 put in!\n");
	    abort();
	}
	qsort(triangles, triangleCount, sizeof(int) * 3,
	    compareTriangle);
	fwdtris = 0;
	rvstris = 0;
	for(j = 0; j < triangleResultsCount; j++) {
	    v1 = triangleResults[j][0];
	    v2 = triangleResults[j][1];
	    v3 = triangleResults[j][2];
	    if(findTriangle(v1, v2, v3)) {
	        fwdtris++;
	    } else {
		if(findTriangle(v2, v1, v3)) { 
		    if(!isBackwardsOkay) {
			printf("oops, couldn't find %u %u %u\n", v1, v2, v3);
			printf("found that one backwards!\n");
			abort();
		    } else
			rvstris++;
		} else {
		    printf("oops, couldn't find %u %u %u\n", v1, v2, v3);
		    abort();
		}
	    }
	} 
	printf("%d added so far to original brk() (%g MB)\n", sbrk(0) - oldBrk,
	    (sbrk(0) - oldBrk) / 1048576.0);
	printf("%d percent backwards triangles\n", rvstris * 100 / 
	    triangleCount);
    }

    actcDelete(tc);
    if(triangleResults != NULL)
        free(triangleResults);
    if(triangles != NULL)
        free(triangles);

    free(timer);

    exit(0);
}

/* vi:tabstop=8
 */
