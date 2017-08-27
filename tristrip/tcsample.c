/*
 * $Header: /home/grantham/cvsroot/projects/modules/tc/tcsample.c,v 1.18 2000/11/22 18:32:02 grantham Exp $
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <signal.h>
#include <unistd.h>
#include "tc.h"

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

void passVertices(FILE *in, FILE *out)
{
    char word[512];
    int done;
    float v[9];
    int vertCount;

    done = 0;
    do {
        if(fscanf(in, "%511s", word) < 1) {
	    fprintf(stderr, "Failed to find either a vert or \"end\"\n");
	    exit(1);
	}
	if(strcmp(word, "end") == 0)
	    done = 1;
	else {
	    if(sscanf(word, "%g", &v[0]) != 1) {
		fprintf(stderr, "Couldn't find number for X in vert %d\n",
		    vertCount);
		exit(1);
	    }
	    if(fscanf(in, "%g %g %g %g %g %g %g %g", &v[1], &v[2], &v[3],
	        &v[4], &v[5], &v[6], &v[7], &v[8]) != 8) {
		fprintf(stderr, "Didn't find complete vert %d\n", vertCount);
		exit(1);
	    }
	    printf("%g %g %g %g %g %g %g %g %g\n", v[0], v[1], v[2], v[3],
	        v[4], v[5], v[6], v[7], v[8]);
	    vertCount++;
	}
    } while(!done);
    fprintf(out, "end\n");
}

void getTriangles(FILE *in, uint *minVert, uint *maxVert)
{
    char word[512];
    int done;
    uint v1, v2, v3;

    if(fscanf(in, "%511s", word) < 1) {
	fprintf(stderr, "Failed to find first prim type \"end\"\n");
	exit(1);
    }
    done = 0;
    triangleCount = 0;
    do { 
	if(strcmp(word, "end") == 0)
	    done = 1;
	else {
	    chartedSetLabel("triangles");
	    triangles = realloc(triangles, (triangleCount + 1) * sizeof(int)
	        * 3);
	    if(triangles == NULL) {
		fprintf(stderr, "Couldn't allocate memory for prim %d\n",
		    triangleCount);
		exit(1);
	    }
	    if(strcmp(word, "triangle") != 0) {
		fprintf(stderr, "Don't know how to handle \"%s\" "
		    "(only know \"triangle\")\n", word);
		exit(1);
	    }
	    if(fscanf(in, "%d %d %d %*s", &v1, &v2, &v3) != 3) {
		fprintf(stderr, "Couldn't read triangle vertices for "
		    "triangle %d\n", triangleCount);
		exit(1);
	    }
	    triangles[triangleCount][0] = v1;
	    triangles[triangleCount][1] = v2;
	    triangles[triangleCount][2] = v3;
	    if(v1 > *maxVert) *maxVert = v1;
	    if(v1 < *minVert) *minVert = v1;
	    if(v2 > *maxVert) *maxVert = v2;
	    if(v2 < *minVert) *minVert = v2;
	    if(v3 > *maxVert) *maxVert = v3;
	    if(v3 < *minVert) *minVert = v3;
	    triangleCount++;
	    if(fscanf(in, "%511s", word) < 1) {
		fprintf(stderr, "Failed to find prim %d type\n", triangleCount);
		exit(1);
	    }
	}
    } while(!done);
}

int main(int argc, char **argv)
{
    int maxBatchSize = INT_MAX;
    ACTCData *tc;
    int j, k, n;
    uint v1, v2, v3;
    int prim;
    int swap;
    int argsused;
    void *oldBrk, *newBrk;
    int doCheckOutput = 1;
    int useOneStopShop = 0;
    int primCount;
    int minVert = UINT_MAX;
    int maxVert = 0;
    size_t bytesAllocated;
    int totalVertexCount;

    tc = actcNew();
    if(tc == NULL) {
	printf("failed to allocate TC structure\n");
	exit(1);
    }

    argc--;
    argv++;
    while(argc > 0) {
	argsused = 1;
	if(strcmp(argv[0], "-h") == 0 || strcmp(argv[0], "-help") == 0) {
	    fprintf(stderr, "tcsample options:\n");
	    fprintf(stderr, "\t-minfanverts #    - set ACTC_MIN_FAN_VERTS\n");
	    fprintf(stderr, "\t-maxprimverts #   - set ACTC_MAX_PRIM_VERTS\n");
	    fprintf(stderr, "\t-honorwinding #   - set ACTC_HONOR_WINDING\n");
	    fprintf(stderr, "\t-onestop          - use TrianglesToPrimives\n");
	    fprintf(stderr, "\t-maxbatch #       - limit # of tris that "
	        "TrianglesToPrimitives can feed\n    in during one pass\n");
	    fprintf(stderr, "\t-nocheck          - don't check accuracy of"
	        "output triangles\n");
	    exit(1);
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
	    CHECKERR(actcParami(tc, ACTC_OUT_HONOR_WINDING, atoi(argv[1])));
	    argsused = 2;
	} else if(strcmp(argv[0], "-nocheck") == 0) {
	    doCheckOutput = 0;
	    argsused = 1;
	} else if(strcmp(argv[0], "-maxbatch") == 0) {
	    if(argc < 2) {
		fprintf(stderr, "option -maxbatch requires an argument\n");
		exit(1);
	    }
	    maxBatchSize = atoi(argv[1]);
	    argsused = 2;
	} else if(strcmp(argv[0], "-onestop") == 0) {
	    useOneStopShop = 1;
	    argsused = 1;
	} else {
	    fprintf(stderr, "didn't understand command line parameter "
		"\"%s\"\n", argv[0]);
	    exit(1);
	}
	argc -= argsused;
	argv += argsused;
    }

    passVertices(stdin, stdout);
    getTriangles(stdin, &minVert, &maxVert);

    chartedSetLabel("triangleResults");
    triangleResults = realloc(triangleResults, sizeof(uint) * 3 *
         triangleCount);

    actcParamu(tc, ACTC_IN_MIN_VERT, minVert);
    actcParamu(tc, ACTC_IN_MAX_VERT, maxVert);

    if(useOneStopShop) {
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
	totalVertexCount = 0;
	for(j = 0; j < primCount; j++) {
	    swap = 0;
	    v1 = primVerts[n++];
	    v2 = primVerts[n++];
	    totalVertexCount += 2;
	    printf("%s %u %u", (primTypes[j] == ACTC_PRIM_STRIP) ?
	        "strip" : "fan", v1, v2);
	    for(k = 2; k < primLengths[j]; k++) {
		totalVertexCount++;
		v3 = primVerts[n++];
		printf(" %u", v3);
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
	    printf("\n");
	}
	fprintf(stderr, "Vertices: %d, primitives: %d, triangles: %d\n",
	    totalVertexCount, primCount, totalVertexCount - 2 * primCount);
	fprintf(stderr, "Vertices per primitive: %g\n", totalVertexCount /
	    (double)primCount);
	fprintf(stderr, "Vertices per triangle: %g\n",
	    totalVertexCount / (double)(totalVertexCount - 2 * primCount));
    } else {
	oldBrk = sbrk(0); /* probably won't compile under Win32 */
	CHECKERR(actcBeginInput(tc))
	for(j = 0; j < triangleCount; j++)
	    CHECKERR(actcAddTriangle(tc, triangles[j][0], triangles[j][1],
                triangles[j][2]));
	CHECKERR(actcEndInput(tc));
	newBrk = sbrk(0);
	CHECKERR(actcGetMemoryAllocation(tc, &bytesAllocated));
	fprintf(stderr, "our brk increased by %u bytes (%g MB) during"
	    " input\n", newBrk - oldBrk, (newBrk - oldBrk) / 1048576.0);
	fprintf(stderr, "TC says it allocated %u bytes (%g MB) during"
	    " input\n", bytesAllocated, bytesAllocated / 1048576.0);

    #if defined(INFO)
	actcDumpState(tc, stderr);
    #endif

	triangleResultsCount = 0;
	primCount = 0;
	totalVertexCount = 0;
	CHECKERR(actcBeginOutput(tc));
	while((prim = actcStartNextPrim(tc, &v1, &v2)) != ACTC_DATABASE_EMPTY) {
	    CHECKERR(prim);
	    primCount ++;
	    totalVertexCount += 2;
	    printf("%s %u %u", (prim == ACTC_PRIM_STRIP) ? "strip" : "fan", v1, v2);
	    swap = 0;
	    while(actcGetNextVert(tc, &v3) != ACTC_PRIM_COMPLETE) {
		CHECKERR(v3);
		totalVertexCount ++;
		printf(" %u", v3);
		if(prim == ACTC_PRIM_STRIP && (swap % 2) == 1){
		    triangleResults[triangleResultsCount][0] = v2;
		    triangleResults[triangleResultsCount][1] = v1;
		    triangleResults[triangleResultsCount][2] = v3;
		} else {
		    triangleResults[triangleResultsCount][0] = v1;
		    triangleResults[triangleResultsCount][1] = v2;
		    triangleResults[triangleResultsCount][2] = v3;
		}
		triangleResultsCount++;
		swap++;
		if(prim == ACTC_PRIM_STRIP) {
		    v1 = v2;
		}
		v2 = v3;
	    }
	    printf(" end\n");
	}
	printf("end\n");
	fprintf(stderr, "Vertices per primitive: %g\n", totalVertexCount /
	    (double)primCount);
	fprintf(stderr, "Triangles per primitive: %g\n",
	    (totalVertexCount - 2 * primCount) / (double)primCount);
	CHECKERR(actcEndOutput(tc));
    }
    if(doCheckOutput) {
	if(triangleResultsCount > triangleCount) {
	    printf("TC gave back more triangles than tcsample put in!\n");
	    abort();
	}
	if(triangleResultsCount < triangleCount) {
	    printf("TC gave back fewer triangles than tcsample put in!\n");
	    abort();
	}
	qsort(triangles, triangleCount, sizeof(int) * 3,
	    compareTriangle);
	for(j = 0; j < triangleResultsCount; j++) {
	    v1 = triangleResults[j][0];
	    v2 = triangleResults[j][1];
	    v3 = triangleResults[j][2];
	    if(!findTriangle(v1, v2, v3)) {
		printf("oops, couldn't find %u %u %u\n", v1, v2, v3);
		if(findTriangle(v2, v1, v3))
		    printf("found that one backwards!\n");
		abort();
	    }
	}
    }

    actcDelete(tc);

    free(triangleResults);
    free(triangles);

    exit(0);
}

/* vi:tabstop=8
 */
