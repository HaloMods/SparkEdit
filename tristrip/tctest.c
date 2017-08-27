/*
 * $Header: /home/grantham/cvsroot/projects/modules/tc/tctest.c,v 1.6 2000/09/20 02:32:52 grantham Exp $
 */

#include <stdio.h>
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

int main(int argc, char **argv)
{
    ACTCData *tc;
    int i;
    uint v1, v2, v3, v4, v5, v6, v7;

    tc = actcNew();
    if(tc == NULL) {
	printf("failed to allocate TC structure\n");
	exit(1);
    }
    i = actcParami(tc, ACTC_OUT_MAX_PRIM_VERTS, 2);
    if(i != ACTC_INVALID_VALUE) {
	printf("didn't get error on MAX_PRIM_VERTS=2 as expected\n");
	exit(1);
    }
    if(i != actcGetError(tc)) {
	printf("error returned didn't equal the error stored\n");
	exit(1);
    }

/* Triangle 1,2,3 */
    CHECKERR(actcBeginInput(tc));
    CHECKERR(actcAddTriangle(tc, 1, 2, 3));
    CHECKERR(actcEndInput(tc));
    CHECKERR(actcBeginOutput(tc));
    CHECKNOT(actcStartNextPrim(tc, &v1, &v2), ACTC_PRIM_STRIP);
    CHECKERR(actcGetNextVert(tc, &v3));
    CHECKNOT(actcGetNextVert(tc, &v4), ACTC_PRIM_COMPLETE);
    CHECKERR(actcEndOutput(tc));
    if(
	! (v1 == 1 && v2 == 2 && v3 == 3) &&
	! (v1 == 2 && v2 == 3 && v3 == 1) &&
	! (v1 == 3 && v2 == 1 && v3 == 2) ) {
	printf("unexpected vertex output:  %u %u %u\n", v1, v2, v3);
	exit(1);
    }

/* Quad 1,2,3,4 */
    CHECKERR(actcBeginInput(tc));
    CHECKERR(actcAddTriangle(tc, 1, 2, 4));
    CHECKERR(actcAddTriangle(tc, 4, 2, 3));
    CHECKERR(actcEndInput(tc));
    CHECKERR(actcBeginOutput(tc));
    CHECKNOT(actcStartNextPrim(tc, &v1, &v2), ACTC_PRIM_STRIP);
    CHECKERR(actcGetNextVert(tc, &v3));
    CHECKERR(actcGetNextVert(tc, &v4));
    CHECKNOT(actcGetNextVert(tc, &v5), ACTC_PRIM_COMPLETE);
    CHECKERR(actcEndOutput(tc));
    /*
     * Next two tests could be fooled by returning same triangle twice,
     * which would certainly be possible, say, if vertices and edges were
     * erroneously not removed from their membership lists.  See tctest2
     * for a more exhaustive and accurate testing framework.
     */
    if(
	! (v1 == 1 && v2 == 2 && v3 == 4) &&
	! (v1 == 2 && v2 == 4 && v3 == 1) &&
	! (v1 == 4 && v2 == 1 && v3 == 2) &&
	! (v1 == 2 && v2 == 3 && v3 == 4) &&
	! (v1 == 3 && v2 == 4 && v3 == 2) &&
	! (v1 == 4 && v2 == 2 && v3 == 3) ) {
	printf("%d : unexpected vertex output:  %u %u %u\n", __LINE__,
	    v1, v2, v3);
	exit(1);
    }
    if(
	! (v2 == 1 && v3 == 4 && v4 == 2) &&
	! (v2 == 2 && v3 == 1 && v4 == 4) &&
	! (v2 == 4 && v3 == 2 && v4 == 1) &&
	! (v2 == 2 && v3 == 4 && v4 == 3) &&
	! (v2 == 3 && v3 == 2 && v4 == 4) &&
	! (v2 == 4 && v3 == 3 && v4 == 2) ) {
	printf("%d : unexpected vertex output:  %u %u %u\n", __LINE__,
	    v2, v3, v4);
	exit(1);
    }

/* independent triangles 1,2,3 and 4,5,6 */
    CHECKERR(actcBeginInput(tc));
    CHECKERR(actcAddTriangle(tc, 1, 2, 3));
    CHECKERR(actcAddTriangle(tc, 4, 5, 6));
    CHECKERR(actcEndInput(tc));
    CHECKERR(actcBeginOutput(tc));
    CHECKNOT(actcStartNextPrim(tc, &v1, &v2), ACTC_PRIM_STRIP);
    CHECKERR(actcGetNextVert(tc, &v3));
    CHECKNOT(actcGetNextVert(tc, &v4), ACTC_PRIM_COMPLETE);
    CHECKNOT(actcStartNextPrim(tc, &v4, &v5), ACTC_PRIM_STRIP);
    CHECKERR(actcGetNextVert(tc, &v6));
    CHECKNOT(actcGetNextVert(tc, &v7), ACTC_PRIM_COMPLETE);
    CHECKERR(actcEndOutput(tc));
    /*
     * Next two tests could be fooled by returning same triangle twice,
     * which would certainly be possible, say, if vertices and edges were
     * erroneously not removed from their membership lists.  See tctest2
     * for a more exhaustive and accurate testing framework.
     */
    if(
	! (v1 == 1 && v2 == 2 && v3 == 3) &&
	! (v1 == 2 && v2 == 3 && v3 == 1) &&
	! (v1 == 3 && v2 == 1 && v3 == 2) &&
	! (v1 == 4 && v2 == 5 && v3 == 6) &&
	! (v1 == 5 && v2 == 6 && v3 == 4) &&
	! (v1 == 6 && v2 == 4 && v3 == 5) ) {
	printf("%d : unexpected vertex output:  %u %u %u\n", __LINE__,
	    v1, v2, v3);
	exit(1);
    }
    if(
	! (v4 == 1 && v5 == 2 && v6 == 3) &&
	! (v4 == 2 && v5 == 3 && v6 == 1) &&
	! (v4 == 3 && v5 == 1 && v6 == 2) &&
	! (v4 == 4 && v5 == 5 && v6 == 6) &&
	! (v4 == 5 && v5 == 6 && v6 == 4) &&
	! (v4 == 6 && v5 == 4 && v6 == 5) ) {
	printf("%d : unexpected vertex output:  %u %u %u\n", __LINE__,
	    v1, v2, v3);
	exit(1);
    }

/* Triangle 1,2,3, but delete it */
    CHECKERR(actcBeginInput(tc));
    CHECKERR(actcAddTriangle(tc, 1, 2, 3));
    CHECKERR(actcEndInput(tc));
    CHECKERR(actcBeginOutput(tc));
    CHECKERR(actcMakeEmpty(tc));
    CHECKNOT(i = actcStartNextPrim(tc, &v1, &v2), ACTC_IDLE);

    actcDelete(tc);

    exit(0);
}

/* vi:tabstop=8
 */
