/* 
** cutout.c
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
    double xcen[3], dx[3], radius;
    TIPSY_HEADER thin, thout;
    GAS_PARTICLE gp;
    DARK_PARTICLE dp;
    STAR_PARTICLE sp;
    ARRAY_HEADER ahin, ahout;
    ARRAY_PARTICLE ap;
    FILE *file1, *file2, *fileindex;
    XDR xdrsin1, xdrsin2, xdrsout1, xdrsout2;

    /*
    ** Set default values
    */
    for (j = 0; j < 3; j++) {
	xcen[j] = 0;
	dx[j] = 0;
	}
    radius = 0;
    fileindex = NULL;
    /*
    ** Read in arguments
    */
    i = 1;
    while (i < argc) {
	if (strcmp(argv[i],"-xcen") == 0) {
	    i++;
	    if (i >= argc) {
		usage();
		}
	    xcen[0] = atof(argv[i]);
	    i++;
	    }
	else if (strcmp(argv[i],"-ycen") == 0) {
	    i++;
	    if (i >= argc) {
		usage();
		}
	    xcen[1] = atof(argv[i]);
	    i++;
	    }
	else if (strcmp(argv[i],"-zcen") == 0) {
	    i++;
	    if (i >= argc) {
		usage();
		}
	    xcen[2] = atof(argv[i]);
	    i++;
	    }
	else if (strcmp(argv[i],"-dx") == 0) {
	    i++;
	    if (i >= argc) {
		usage();
		}
	    dx[0] = atof(argv[i]);
	    i++;
	    }
	else if (strcmp(argv[i],"-dy") == 0) {
	    i++;
	    if (i >= argc) {
		usage();
		}
	    dx[1] = atof(argv[i]);
	    i++;
	    }
	else if (strcmp(argv[i],"-dz") == 0) {
	    i++;
	    if (i >= argc) {
		usage();
		}
	    dx[2] = atof(argv[i]);
	    i++;
	    }
	else if (strcmp(argv[i],"-r") == 0) {
	    i++;
	    if (i >= argc) {
		usage();
		}
	    radius = atof(argv[i]);
	    i++;
	    }
        else if (strcmp(argv[i],"-i") == 0) {
            i++;
            if (i >= argc) {
                usage();
                }
            fileindex = fopen(argv[i],"w");
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
    ** Check some things
    */
    if (radius == 0) {
	for (j = 0; j < 3; j++) {
	    assert(dx[j] != 0);
	    }
	}
    else {
	for (j = 0; j < 3; j++) {
	    assert(dx[j] == 0);
	    }
	}
    assert(fileindex != NULL);
    /*
    ** Read in header
    */
    xdrstdio_create(&xdrsin1,stdin,XDR_DECODE);
    read_tipsy_standard_header(&xdrsin1,&thin);
    /*
    ** Write out temporary file to be replaced later
    */
    file1 = fopen("temporary_file_1","w");
    assert(file1 != NULL);
    xdrstdio_create(&xdrsout1,file1,XDR_ENCODE);
    thout.time = thin.time;
    thout.ntotal = 0;
    thout.ndim = thin.ndim;
    thout.ngas = 0;
    thout.ndark = 0;
    thout.nstar = 0;
    write_tipsy_standard_header(&xdrsout1,&thout);
    file2 = fopen("temporary_file_2","w");
    assert(file2 != NULL);
    xdrstdio_create(&xdrsout2,file2,XDR_ENCODE);
    ahout.N[0] = 0;
    ahout.N[1] = 1;
    ahout.N[2] = 0;
    ahout.N[3] = 0;
    allocate_array_particle(&ahout,&ap);
    write_array_header(&xdrsout2,&ahout);
    /*
    ** Check if particles are written out
    */
    if (radius == 0) {
	for (i = 0; i < thin.ngas; i++) {
	    read_tipsy_standard_gas(&xdrsin1,&gp);
	    if (fabs(gp.pos[0]-xcen[0]) <= dx[0]/2.0 &&
		fabs(gp.pos[1]-xcen[1]) <= dx[1]/2.0 &&
		fabs(gp.pos[2]-xcen[2]) <= dx[2]/2.0) {
		thout.ngas++;
		write_tipsy_standard_gas(&xdrsout1,&gp);
		ap.ia[0] = 1+i;
		write_array_particle(&xdrsout2,&ahout,&ap);
		}
	    }
	for (i = 0; i < thin.ndark; i++) {
	    read_tipsy_standard_dark(&xdrsin1,&dp);
	    if (fabs(dp.pos[0]-xcen[0]) <= dx[0]/2.0 &&
		fabs(dp.pos[1]-xcen[1]) <= dx[1]/2.0 &&
		fabs(dp.pos[2]-xcen[2]) <= dx[2]/2.0) {
		thout.ndark++;
		write_tipsy_standard_dark(&xdrsout1,&dp);
		ap.ia[0] = thin.ngas+1+i;
		write_array_particle(&xdrsout2,&ahout,&ap);
		}
	    }
	for (i = 0; i < thin.nstar; i++) {
	    read_tipsy_standard_star(&xdrsin1,&sp);
	    if (fabs(dp.pos[0]-xcen[0]) <= dx[0]/2.0 &&
		fabs(dp.pos[1]-xcen[1]) <= dx[1]/2.0 &&
		fabs(dp.pos[2]-xcen[2]) <= dx[2]/2.0) {
		thout.nstar++;
		write_tipsy_standard_star(&xdrsout1,&sp);
		ap.ia[0] = thin.ngas+thin.ndark+1+i;
		write_array_particle(&xdrsout2,&ahout,&ap);
		}
	    }
	}
    thout.ntotal = thout.ngas + thout.ndark + thout.nstar;
    ahout.N[0] = thout.ntotal;
    xdr_destroy(&xdrsin1);
    xdr_destroy(&xdrsout1);
    fclose(file1);
    fclose(file2);
    /*
    ** Now read temporary files again and correct header
    */
    file1 = fopen("temporary_file_1","r");
    file2 = fopen("temporary_file_2","r");
    xdrstdio_create(&xdrsin1,file1,XDR_DECODE);
    xdrstdio_create(&xdrsin2,file2,XDR_DECODE);
    xdrstdio_create(&xdrsout1,stdout,XDR_ENCODE);
    xdrstdio_create(&xdrsout2,fileindex,XDR_ENCODE);
    read_tipsy_standard_header(&xdrsin1,&thin);
    write_tipsy_standard_header(&xdrsout1,&thout);
    read_array_header(&xdrsin2,&ahin);
    write_array_header(&xdrsout2,&ahout);
    for (i = 0; i < thout.ngas; i++) { 
	read_tipsy_standard_gas(&xdrsin1,&gp);
	write_tipsy_standard_gas(&xdrsout1,&gp);
	read_array_particle(&xdrsin2,&ahout,&ap);
	write_array_particle(&xdrsout2,&ahout,&ap);
	}
    for (i = 0; i < thout.ndark; i++) {
	read_tipsy_standard_dark(&xdrsin1,&dp);
	write_tipsy_standard_dark(&xdrsout1,&dp);
	read_array_particle(&xdrsin2,&ahout,&ap);
	write_array_particle(&xdrsout2,&ahout,&ap);
	}
    for (i = 0; i < thout.nstar; i++) {
	read_tipsy_standard_star(&xdrsin1,&sp);
	write_tipsy_standard_star(&xdrsout1,&sp);
	read_array_particle(&xdrsin2,&ahout,&ap);
	write_array_particle(&xdrsout2,&ahout,&ap);
	}
    xdr_destroy(&xdrsin1);
    xdr_destroy(&xdrsin2);
    xdr_destroy(&xdrsout1);
    xdr_destroy(&xdrsout2);
    fprintf(stderr,"Time: %g Ntotal: %d Ngas: %d Ndark: %d Nstar: %d Ndim: %d (input)\n",
	    thin.time,thin.ntotal,thin.ngas,thin.ndark,thin.nstar,thin.ndim);
    fprintf(stderr,"Time: %g Ntotal: %d Ngas: %d Ndark: %d Nstar: %d Ndim: %d (output)\n",
	    thout.time,thout.ntotal,thout.ngas,thout.ndark,thout.nstar,thout.ndim);
    exit(0);
    }

void usage(void) {

    fprintf(stderr,"\n");
    fprintf(stderr,"Cutout cuts out regions from input file.\n\n");
    fprintf(stderr,"You can specify the following arguments:\n\n");
    fprintf(stderr,"-xcen <value> : x-coordinate of centre [LU] (default: 0 LU)\n");
    fprintf(stderr,"-ycen <value> : y-coordinate of centre [LU] (default: 0 LU)\n");
    fprintf(stderr,"-zcen <value> : z-coordinate of centre [LU] (default: 0 LU)\n");
    fprintf(stderr,"-dx <value>   : box side length in x-direction [LU] (default: 0 LU)\n");
    fprintf(stderr,"-dy <value>   : box side length in y-direction [LU] (default: 0 LU)\n");
    fprintf(stderr,"-dz <value>   : box side length in z-direction [LU] (default: 0 LU)\n");
    fprintf(stderr,"-i <name>     : output index file\n");
    fprintf(stderr,"< <name>      : input file in tipsy standard binary format\n");
    fprintf(stderr,"> <name>      : output file in tipsy standard binary format\n");
    exit(1);
    }