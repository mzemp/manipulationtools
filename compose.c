/* 
** compose.c
**
** written by Marcel Zemp
*/

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <assert.h>
#include <rpc/types.h>
#include <rpc/xdr.h>
#include <iof.h>

void usage(void);

int main(int argc, char **argv) {

    int i, j;
    double dx1[3], dx2[3], dv1[3], dv2[3];
    FILE *fp1, *fp2;
    TIPSY_HEADER th1, th2, thout;
    TIPSY_GAS_PARTICLE gp;
    TIPSY_DARK_PARTICLE dp;
    TIPSY_STAR_PARTICLE sp;
    XDR xdr1, xdr2, xdrout;

    fp1 = NULL;
    fp2 = NULL;

    /*
    ** Set default values
    */

    for (j = 0; j < 3; j++) {
	dx1[j] = 0;
	dx2[j] = 0;
	dv1[j] = 0;
	dv2[j] = 0;
	}

    /*
    ** Read in arguments
    */

    i = 1;
    while (i < argc) {
	if (strcmp(argv[i],"-f1") == 0) {
	    i++;
	    if (i >= argc) {
		usage();
		}
	    fp1 = fopen(argv[i],"r");
	    i++;
	    }
	else if (strcmp(argv[i],"-f2") == 0) {
	    i++;
	    if (i >= argc) {
		usage();
		}
	    fp2 = fopen(argv[i],"r");
	    i++;
	    }
	else if (strcmp(argv[i],"-drx1") == 0) {
	    i++;
	    if (i >= argc) {
		usage();
		}
	    dx1[0] = atof(argv[i]);
	    i++;
	    }
	else if (strcmp(argv[i],"-dry1") == 0) {
	    i++;
	    if (i >= argc) {
		usage();
		}
	    dx1[1] = atof(argv[i]);
	    i++;
	    }
	else if (strcmp(argv[i],"-drz1") == 0) {
	    i++;
	    if (i >= argc) {
		usage();
		}
	    dx1[2] = atof(argv[i]);
	    i++;
	    }
	else if (strcmp(argv[i],"-drx2") == 0) {
	    i++;
	    if (i >= argc) {
		usage();
		}
	    dx2[0] = atof(argv[i]);
	    i++;
	    }
	else if (strcmp(argv[i],"-dry2") == 0) {
	    i++;
	    if (i >= argc) {
		usage();
		}
	    dx2[1] = atof(argv[i]);
	    i++;
	    }
	else if (strcmp(argv[i],"-drz2") == 0) {
	    i++;
	    if (i >= argc) {
		usage();
		}
	    dx2[2] = atof(argv[i]);
	    i++;
	    }
	else if (strcmp(argv[i],"-dvx1") == 0) {
	    i++;
	    if (i >= argc) {
		usage();
		}
	    dv1[0] = atof(argv[i]);
	    i++;
	    }
	else if (strcmp(argv[i],"-dvy1") == 0) {
	    i++;
	    if (i >= argc) {
		usage();
		}
	    dv1[1] = atof(argv[i]);
	    i++;
	    }
	else if (strcmp(argv[i],"-dvz1") == 0) {
	    i++;
	    if (i >= argc) {
		usage();
		}
	    dv1[2] = atof(argv[i]);
	    i++;
	    }
	else if (strcmp(argv[i],"-dvx2") == 0) {
	    i++;
	    if (i >= argc) {
		usage();
		}
	    dv2[0] = atof(argv[i]);
	    i++;
	    }
	else if (strcmp(argv[i],"-dvy2") == 0) {
	    i++;
	    if (i >= argc) {
		usage();
		}
	    dv2[1] = atof(argv[i]);
	    i++;
	    }
	else if (strcmp(argv[i],"-dvz2") == 0) {
	    i++;
	    if (i >= argc) {
		usage();
		}
	    dv2[2] = atof(argv[i]);
	    i++;
	    }
	else if ((strcmp(argv[i],"-h") == 0) || (strcmp(argv[i],"-help") == 0)) {
	    usage();
	    }
	else {
	    usage();
	    }
	}

    /*
    ** Read in headers
    */

    assert(fp1 != NULL);
    assert(fp2 != NULL);
    xdrstdio_create(&xdr1,fp1,XDR_DECODE);
    xdrstdio_create(&xdr2,fp2,XDR_DECODE);
    read_tipsy_xdr_header(&xdr1,&th1);
    read_tipsy_xdr_header(&xdr2,&th2);

    /*
    ** Write out new header
    */

    xdrstdio_create(&xdrout,stdout,XDR_ENCODE);
    thout.time = th1.time;
    thout.ntotal = th1.ntotal + th2.ntotal;
    thout.ndim = th1.ndim;
    thout.ngas = th1.ngas + th2.ngas;
    thout.ndark = th1.ndark + th2.ndark;
    thout.nstar = th1.nstar + th2.nstar;
    write_tipsy_xdr_header(&xdrout,&thout);

    /*
    ** Add shifts to particles and write them out
    */

    for(i = 0; i < th1.ngas; i++) {
	read_tipsy_xdr_gas(&xdr1,&gp);
	for(j = 0; j < 3; j++) {
	    gp.pos[j] = gp.pos[j]+dx1[j];
	    gp.vel[j] = gp.vel[j]+dv1[j];
	    }
	write_tipsy_xdr_gas(&xdrout,&gp);
	}
    for(i = 0; i < th2.ngas; i++) {
	read_tipsy_xdr_gas(&xdr2,&gp);
	for(j = 0; j < 3; j++) {
	    gp.pos[j] = gp.pos[j]+dx2[j];
	    gp.vel[j] = gp.vel[j]+dv2[j];
	    }
	write_tipsy_xdr_gas(&xdrout,&gp);
	}
    for(i = 0; i < th1.ndark; i++) {
	read_tipsy_xdr_dark(&xdr1,&dp);
	for(j = 0; j < 3; j++) {
	    dp.pos[j] = dp.pos[j]+dx1[j];
	    dp.vel[j] = dp.vel[j]+dv1[j];
	    }
	write_tipsy_xdr_dark(&xdrout,&dp);
	}
    for(i = 0; i < th2.ndark; i++) {
	read_tipsy_xdr_dark(&xdr2,&dp);
	for(j = 0; j < 3; j++) {
	    dp.pos[j] = dp.pos[j]+dx2[j];
	    dp.vel[j] = dp.vel[j]+dv2[j];
	    }
	write_tipsy_xdr_dark(&xdrout,&dp);
	}
    for(i = 0; i < th1.nstar; i++) {
	read_tipsy_xdr_star(&xdr1,&sp);
	for(j = 0; j < 3; j++) {
	    sp.pos[j] = sp.pos[j]+dx1[j];
	    sp.vel[j] = sp.vel[j]+dv1[j];
	    }
	write_tipsy_xdr_star(&xdrout,&sp);
	}
    for(i = 0; i < th2.nstar; i++) {
	read_tipsy_xdr_star(&xdr2,&sp);
	for(j = 0; j < 3; j++) {
	    sp.pos[j] = sp.pos[j]+dx2[j];
	    sp.vel[j] = sp.vel[j]+dv2[j];
	    }
	write_tipsy_xdr_star(&xdrout,&sp);
	}

    xdr_destroy(&xdr1);
    xdr_destroy(&xdr1);    
    xdr_destroy(&xdrout);

    fprintf(stderr,"Structure 1:\n");
    fprintf(stderr,"Time: %g Ntotal: %d Ngas: %d Ndark: %d Nstar: %d Ndim: %d\n",
	    th1.time,th1.ntotal,th1.ngas,th1.ndark,th1.nstar,th1.ndim);
    fprintf(stderr,"(drx1,dry1,drz1) = (%g,%g,%g) LU / (dvx1,dvy1,dvz1) = (%g,%g,%g) VU\n",
	    dx1[0],dx1[1],dx1[2],dv1[0],dv1[1],dv1[2]);
    fprintf(stderr,"Structure 2:\n");
    fprintf(stderr,"Time: %g Ntotal: %d Ngas: %d Ndark: %d Nstar: %d Ndim: %d\n",
	    th2.time,th2.ntotal,th2.ngas,th2.ndark,th2.nstar,th2.ndim);
    fprintf(stderr,"(drx2,dry2,drz2) = (%g,%g,%g) LU / (dvx2,dvy2,dvz2) = (%g,%g,%g) VU\n",
	    dx2[0],dx2[1],dx2[2],dv2[0],dv2[1],dv2[2]);
    fprintf(stderr,"Output:\n");
    fprintf(stderr,"Time: %g Ntotal: %d Ngas: %d Ndark: %d Nstar: %d Ndim: %d\n",
	    thout.time,thout.ntotal,thout.ngas,thout.ndark,thout.nstar,thout.ndim);

    exit(0);
    }

void usage(void) {

    fprintf(stderr,"\n");
    fprintf(stderr,"Compose adds structures of two input files and shifts each structure by the specified\n");
    fprintf(stderr,"distance and velocity shifts and writes an output.\n\n");
    fprintf(stderr,"You can specify the following arguments:\n\n");
    fprintf(stderr,"-f1 <name>    : name of input file 1 (Tipsy standard binary)\n");
    fprintf(stderr,"-f2 <name>    : name of input file 2 (Tipsy standard binary)\n");
    fprintf(stderr,"-drx1 <value> : position shift along x-axis for file 1 (default: 0 LU)\n");
    fprintf(stderr,"-dry1 <value> : position shift along y-axis for file 1 (default: 0 LU)\n");
    fprintf(stderr,"-drz1 <value> : position shift along z-axis for file 1 (default: 0 LU)\n");
    fprintf(stderr,"-drx2 <value> : position shift along x-axis for file 2 (default: 0 LU)\n");
    fprintf(stderr,"-dry2 <value> : position shift along y-axis for file 2 (default: 0 LU)\n");
    fprintf(stderr,"-drz2 <value> : position shift along z-axis for file 2 (default: 0 LU)\n");
    fprintf(stderr,"-dvx1 <value> : velocity shift along x-axis for file 1 (default: 0 VU)\n");
    fprintf(stderr,"-dvy1 <value> : velocity shift along y-axis for file 1 (default: 0 VU)\n");
    fprintf(stderr,"-dvz1 <value> : velocity shift along z-axis for file 1 (default: 0 VU)\n");
    fprintf(stderr,"-dvx2 <value> : velocity shift along x-axis for file 2 (default: 0 VU)\n");
    fprintf(stderr,"-dvy2 <value> : velocity shift along y-axis for file 2 (default: 0 VU)\n");
    fprintf(stderr,"-dvz2 <value> : velocity shift along z-axis for file 2 (default: 0 VU)\n");
    fprintf(stderr,"> <name>      : name of outputfile (Tipsy standard binary)\n");
    fprintf(stderr,"\n");
    exit(1);
    }
