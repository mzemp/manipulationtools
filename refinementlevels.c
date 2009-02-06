/* 
** refinementlevels.c
**
** Program written in order to determine refinement levels
**
** written by Marcel Zemp
*/

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <assert.h>
#include <iof.h>
#include <rpc/types.h>
#include <rpc/xdr.h>

typedef struct group {

    int index;
    double mass;
    double size;
    double rx;
    double ry;
    double rz;
    } GROUP;

double correct_position(double c, double r, double l) {

    if (c > 0.25*l && r < -0.25*l) {
        return r + l;
        }
    else if (c < -0.25*l && r > 0.25*l) {
        return r - l;
        }
    else {
        return r;
        }
    }

void usage(void);

int main(int argc, char **argv) {

    int i, j, id;
    int idummy;
    int positionprecision;
    int refinementtype;
    int SizeGroupArray = 100, NGroupRead = 0;
    int *refinementlevel = NULL;
    float fdummy;
    float size, mass, rx, ry, rz, r;
    char statsfilename[256], statisticsfilename[256], groupsfilename[256];
    double drvir;
    TIPSY_HEADER th;
    GAS_PARTICLE gp;
    DARK_PARTICLE dp;
    STAR_PARTICLE sp;
    GAS_PARTICLE_DPP gpdpp;
    DARK_PARTICLE_DPP dpdpp;
    STAR_PARTICLE_DPP spdpp;
    XDR xdrs;
    GROUP *group = NULL;
    FILE *statsfile, *statisticsfile, *groupsfile;

    positionprecision = 0; /* spp 0, dpp 1 */
    refinementtype = 0;

    /*
    ** Read in arguments
    */
    i = 1;
    while (i < argc) {
	if (strcmp(argv[i],"-spp") == 0) {
            positionprecision = 0;
            i++;
            }
        else if (strcmp(argv[i],"-dpp") == 0) {
            positionprecision = 1;
            i++;
            }
        else if (strcmp(argv[i],"-statistics") == 0) {
            i++;
            if (i >= argc) {
                usage();
                }
            strcpy(statisticsfilename,argv[i]);
            i++;
            }
        else if (strcmp(argv[i],"-stats") == 0) {
            i++;
            if (i >= argc) {
                usage();
                }
            strcpy(statsfilename,argv[i]);
            i++;
            }
        else if (strcmp(argv[i],"-groups") == 0) {
            i++;
            if (i >= argc) {
                usage();
                }
            strcpy(groupsfilename,argv[i]);
            i++;
            }
        else if (strcmp(argv[i],"-rt") == 0) {
            i++;
            if (i >= argc) {
                usage();
                }
	    refinementtype = atoi(argv[i]);
            i++;
            }
        else if (strcmp(argv[i],"-d") == 0) {
            i++;
            if (i >= argc) {
                usage();
                }
	    drvir= atof(argv[i]);
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
    ** Read in indices of groups from group file
    */
    group = realloc(group,SizeGroupArray*sizeof(GROUP));
    groupsfile = fopen(groupsfilename,"r");
    while (1) {
        fscanf(groupsfile,"%i",&idummy); id = idummy;
        if (feof(groupsfile)) break;
        NGroupRead++;
        if (NGroupRead == SizeGroupArray) {
            SizeGroupArray += 100;
	    group = realloc(group,SizeGroupArray*sizeof(GROUP));
            }
	group[NGroupRead-1].index = id;
	group[NGroupRead-1].size = 0;
	group[NGroupRead-1].mass = 0;
	group[NGroupRead-1].rx = 0;
	group[NGroupRead-1].ry = 0;
	group[NGroupRead-1].rz = 0;
        }
    /*
    ** Get masses and sizes from statistics file
    */
    statisticsfile = fopen(statisticsfilename,"r");
    while (1) {
        fscanf(statisticsfile,"%i",&idummy); id = idummy;
	fscanf(statisticsfile,"%g",&fdummy); size = fdummy;
	fscanf(statisticsfile,"%g",&fdummy); mass = fdummy;
	fscanf(statisticsfile,"%g",&fdummy);
	fscanf(statisticsfile,"%g",&fdummy);
	fscanf(statisticsfile,"%g",&fdummy);
	fscanf(statisticsfile,"%g",&fdummy);
	fscanf(statisticsfile,"%g",&fdummy);
	if (feof(statisticsfile)) break;
	for (i = 0; i < NGroupRead; i++) {
	    if (id == group[i].index) {
		group[i].size = size;
		group[i].mass = mass;
		}
	    }
	}
    fclose(statisticsfile);
    /*
    ** Get centres from stats file
    */
    statsfile = fopen(statsfilename,"r");
    while (1) {
	fscanf(statsfile,"%i",&idummy); id = idummy;
	fscanf(statsfile,"%i",&idummy);
	fscanf(statsfile,"%g",&fdummy);
	fscanf(statsfile,"%g",&fdummy);
	fscanf(statsfile,"%g",&fdummy);
	fscanf(statsfile,"%g",&fdummy);
	fscanf(statsfile,"%g",&fdummy);
	fscanf(statsfile,"%g",&fdummy);
	fscanf(statsfile,"%g",&fdummy);
	fscanf(statsfile,"%g",&fdummy);
	fscanf(statsfile,"%g",&fdummy);
	fscanf(statsfile,"%g",&fdummy);
	fscanf(statsfile,"%g",&fdummy);
	fscanf(statsfile,"%g",&fdummy);
	fscanf(statsfile,"%g",&fdummy);
	fscanf(statsfile,"%g",&fdummy);
	fscanf(statsfile,"%g",&fdummy);
	fscanf(statsfile,"%g",&fdummy);
	fscanf(statsfile,"%g",&fdummy); rx = fdummy;
	fscanf(statsfile,"%g",&fdummy); ry = fdummy;
	fscanf(statsfile,"%g",&fdummy); rz = fdummy;
	fscanf(statsfile,"%g",&fdummy);
	fscanf(statsfile,"%g",&fdummy);
	fscanf(statsfile,"%g",&fdummy);
	fscanf(statsfile,"%g",&fdummy);
	fscanf(statsfile,"%g",&fdummy);
	fscanf(statsfile,"%g",&fdummy);
	fscanf(statsfile,"%g",&fdummy);
	fscanf(statsfile,"%g",&fdummy);
	if (feof(statsfile)) break;
	for (i = 0; i < NGroupRead; i++) {
	    if (id == group[i].index) {
		group[i].rx = rx;
		group[i].ry = ry;
		group[i].rz = rz;
		}
	    }
	}
    fclose(statsfile);
    /*
    ** Go through all particles in file and determine their refinement level
    */
    xdrstdio_create(&xdrs,stdin,XDR_DECODE);
    read_tipsy_standard_header(&xdrs,&th);
    assert(th.ngas == 0);
    assert(th.nstar == 0);
    assert(positionprecision == 0);
    refinementlevel = realloc(refinementlevel,th.ntotal*sizeof(int));
    for (i = 0; i < th.ntotal; i++) {
	if (refinementtype == 0) {
	    refinementlevel[i] = 0;
	    }
	else if (refinementtype == 1) {
	    refinementlevel[i] = 10;
	    }
	}
    if (positionprecision == 0) {
	for (i = 0; i < th.ngas; i++) {
	    read_tipsy_standard_gas(&xdrs,&gp);
	    }
	for (i = 0; i < th.ndark; i++) {
	    read_tipsy_standard_dark(&xdrs,&dp);
	    for (j = 0; j < NGroupRead; j++) {
		rx = correct_position(group[j].rx,dp.pos[0],1);
		ry = correct_position(group[j].ry,dp.pos[1],1);
		rz = correct_position(group[j].rz,dp.pos[2],1);
		r = (rx-group[j].rx)*(rx-group[j].rx);
		r += (ry-group[j].ry)*(ry-group[j].ry);
		r += (rz-group[j].rz)*(rz-group[j].rz);
		r = sqrt(r);
		if (refinementtype == 0) {
		    if (r <= drvir*group[j].size) {
			refinementlevel[i] = 3;
			}
		    else if ((r > drvir*group[j].size) && (r <= (drvir+1.0)*group[j].size)) {
			if (refinementlevel[i] < 2) {
			    refinementlevel[i] = 2;
			    }
			}
		    else if ((r > (drvir+1.0)*group[j].size) && (r <= (drvir+2.0)*group[j].size)) {
			if (refinementlevel[i] < 1) {
			    refinementlevel[i] = 1;
			    }
			}
		    }
		else if (refinementtype == 1) {
		    if (r <= 1*group[j].size) {
			refinementlevel[i] = 1;
			}
		    else if ((r > 1*group[j].size) && (r <= 2*group[j].size)) {
			if (refinementlevel[i] > 2) {
			    refinementlevel[i] = 2;
			    }
			}
		    else if ((r > 2*group[j].size) && (r <= 3*group[j].size)) {
			if (refinementlevel[i] > 3) {
			    refinementlevel[i] = 3;
			    }
			}
		    else if ((r > 3*group[j].size) && (r <= 4*group[j].size)) {
			if (refinementlevel[i] > 4) {
			    refinementlevel[i] = 4;
			    }
			}
		    else if ((r > 4*group[j].size) && (r <= 5*group[j].size)) {
			if (refinementlevel[i] > 5) {
			    refinementlevel[i] = 5;
			    }
			}
		    else if ((r > 5*group[j].size) && (r <= 6*group[j].size)) {
			if (refinementlevel[i] > 6) {
			    refinementlevel[i] = 6;
			    }
			}
		    else if ((r > 6*group[j].size) && (r <= 7*group[j].size)) {
			if (refinementlevel[i] > 7) {
			    refinementlevel[i] = 7;
			    }
			}
		    else if ((r > 7*group[j].size) && (r <= 8*group[j].size)) {
			if (refinementlevel[i] > 8) {
			    refinementlevel[i] = 8;
			    }
			}
		    else if ((r > 8*group[j].size) && (r <= 9*group[j].size)) {
			if (refinementlevel[i] > 9) {
			    refinementlevel[i] = 9;
			    }
			}
		    }
		}
	    }
	for (i = 0; i < th.nstar; i++) {
	    read_tipsy_standard_star(&xdrs,&sp);
	    }
	}
    else if (positionprecision == 1) {
	for (i = 0; i < th.ngas; i++) {
	    read_tipsy_standard_gas_dpp(&xdrs,&gpdpp);
	    }
	for (i = 0; i < th.ndark; i++) {
	    read_tipsy_standard_dark_dpp(&xdrs,&dpdpp);
	    }
	for (i = 0; i < th.nstar; i++) {
	    read_tipsy_standard_star_dpp(&xdrs,&spdpp);
	    }
	}
    xdr_destroy(&xdrs);
    /*
    ** Write refinement levels as tipsy ascii array
    */
    fprintf(stdout,"%d\n",th.ntotal);
    for (i = 0; i < th.ntotal; i++) {
	fprintf(stdout,"%d\n",refinementlevel[i]);
	}
    exit(0);
    }

void usage(void) {

    fprintf(stderr,"\n");
    fprintf(stderr,"Program calculates the refinement levels of the particles.\n");
    fprintf(stderr,"\n");
    fprintf(stderr,"You can specify the following arguments:\n");
    fprintf(stderr,"\n");
    fprintf(stderr,"-spp               : set this flag if input and output files have single precision positions (default)\n");
    fprintf(stderr,"-dpp               : set this flag if input and output files have double precision positions\n");
    fprintf(stderr,"-stats <name>      : stats file\n");
    fprintf(stderr,"-statistics <name> : statistics file\n");
    fprintf(stderr,"-groups <name>     : groups file\n");
    fprintf(stderr,"-rt <value>        : refinement type: 0: high-resolution region given by d 1: particles have index according to distance to nearest group in units of rvir of that group\n");
    fprintf(stderr,"-d <value>         : size of high-resolution region in units of rvir (only for rt=0)\n");
    fprintf(stderr,"< <name>           : name of input file in tipsy standard binary format\n");
    fprintf(stderr,"> <name>           : name of output file in tipsy ascii array format\n");
    fprintf(stderr,"\n");
    exit(1);
    }
