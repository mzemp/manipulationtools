/* 
** smoothrefinementlevels.c
**
** written by Marcel Zemp
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <assert.h>
#include <math.h>
#include <iof.h>

void usage(void);

int put_index_in_box(int Ngrid, int index) {

    if (index < 0) {
	index += Ngrid;
	assert(index >= 0);
	}
    return (index%Ngrid);
    }

void find_next(int ***rl, int Ngrid, int level, int isel, int jsel, int ksel, int *inext, int *jnext, int *knext) {

    int i,j,k,l;

    for (i = isel+1; i < Ngrid+isel; i++) {
	l = put_index_in_box(Ngrid,i);
	if (rl[l][jsel][ksel] == level) {
	    *inext = l;
	    break;
	    }
	}
    for (j = jsel+1; j < Ngrid+jsel; j++) {
	l = put_index_in_box(Ngrid,j);
	if (rl[isel][l][ksel] == level) {
	    *jnext = l;
	    break;
	    }
	}
    for (k = ksel+1; k < Ngrid+ksel; k++) {
	l = put_index_in_box(Ngrid,k);
	if (rl[isel][jsel][l] == level) {
	    *knext = l;
	    break;
	    }
	}
    }

int main(int argc, char **argv) {

    int i,j,k,l,m,n;
    int ifirst,jfirst,kfirst;
    int inext,jnext,knext;
    int ltemp,ntemp,mtemp;
    int foundfirst;
    int idummy;
    int Ntot,Ngrid;
    int lmax,Ngap,Nsingle;
    int verboselevel;
    int ***rl = NULL;
    double meanlevel;

    verboselevel = 0;
    lmax = 0;
    ifirst = 0;
    jfirst = 0;
    kfirst = 0;
    inext = 0;
    jnext = 0;
    knext = 0;
    foundfirst = 0;
    i = 1;
    while (i < argc) {
	if (strcmp(argv[i],"-lmax") == 0) {
            i++;
            if (i >= argc) {
                usage();
                }
	    lmax = atoi(argv[i]);
            i++;
            }
	else if (strcmp(argv[i],"-Ngap") == 0) {
            i++;
            if (i >= argc) {
                usage();
                }
	    Ngap = atoi(argv[i]);
            i++;
            }
	else if (strcmp(argv[i],"-v") == 0) {
	    verboselevel = 1;
	    i++;
	    }
	else if ((strcmp(argv[i],"-h") == 0) || (strcmp(argv[i],"-help") == 0)) {
            usage();
            }
        else {
            usage();
            }
        }
    assert(lmax >=0);
    /*
    ** Read in total number of particles and allocate & initialise array
    */
    assert(fscanf(stdin,"%d",&Ntot) == 1);
    Ngrid = (int) cbrt(Ntot);
    assert(Ngrid*Ngrid*Ngrid == Ntot);
    rl = realloc(rl,Ngrid*sizeof(int **));
    assert(rl != NULL);
    for (i = 0; i < Ngrid; i++) {
        rl[i] = realloc(rl[i],Ngrid*sizeof(int *));
        assert(rl[i] != NULL);
        for (j = 0; j < Ngrid; j++) {
	    rl[i][j] = realloc(rl[i][j],Ngrid*sizeof(int));
	    for (k = 0; k < Ngrid; k++) {
		rl[i][j][k] = 0;
		}
	    }
	}
    /*
    ** Read refinement levels of particles
    */
    for (k = 0; k < Ngrid; k++) {
	for (j = 0; j < Ngrid; j++) {
	    for (i = 0; i < Ngrid; i++) {
		assert(fscanf(stdin,"%d",&idummy) == 1);
		rl[i][j][k] = idummy;
		if (rl[i][j][k] == lmax && foundfirst == 0) {
		    ifirst = i;
		    jfirst = j;
		    kfirst = k;
		    foundfirst = 1;
		    }
		}
	    }
	}
    /*
    ** Go through particles and find gaps
    */
    for (k = kfirst; k < Ngrid; k++) {
	for (j = jfirst; j < Ngrid; j++) {
	    for (i = ifirst; i < Ngrid; i++) {
		if (rl[i][j][k] != lmax) continue;
		inext = jnext = knext = 0;
		find_next(rl,Ngrid,lmax,i,j,k,&inext,&jnext,&knext);
		if (inext <= i) inext += Ngrid;
		if (jnext <= j) jnext += Ngrid;
		if (knext <= k) knext += Ngrid;
		if (inext-i <= Ngap+1) {
		    for (l = i+1; l < inext; l++) {
			m = put_index_in_box(Ngrid,l);
			rl[m][j][k] = lmax;
			}
		    }
		if (jnext-j <= Ngap+1) {
		    for (l = j+1; l < jnext; l++) {
			m = put_index_in_box(Ngrid,l);
			rl[i][m][k] = lmax;
			}
		    }
		if (knext-k <= Ngap+1) {
		    for (l = k+1; l < knext; l++) {
			m = put_index_in_box(Ngrid,l);
			rl[i][j][m] = lmax;
			}
		    }
		}
	    }
	}
    /*
    ** Remove single particles
    */
    Nsingle = 0;
    for (k = 0; k < Ngrid; k++) {
	for (j = 0; j < Ngrid; j++) {
	    for (i = 0; i < Ngrid; i++) {
		if (rl[i][j][k] != lmax) continue;
		meanlevel = 0;
		for (l = i-1; l <= i+1; l++) {
		    for (m = j-1; m <= j+1; m++) {
			for (n = k-1; n <= k+1; n++) {
			    ltemp = put_index_in_box(Ngrid,l);
			    mtemp = put_index_in_box(Ngrid,m);
			    ntemp = put_index_in_box(Ngrid,n);
			    if (ltemp == i && mtemp == j && ntemp == k) continue;
			    if (rl[ltemp][mtemp][ntemp] == lmax) {
				goto outloop;
				}
			    else {
				meanlevel += rl[ltemp][mtemp][ntemp];
				}
			    }
			}
		    }
		meanlevel /= 28.0;
		Nsingle++;
		rl[i][j][k] = (int) (meanlevel+0.5);
	    outloop:
		continue;
		}
	    }
	}
    /*
    ** Write out new array
    */
    fprintf(stdout,"%d\n",Ntot);
    for (k = 0; k < Ngrid; k++) {
	for (j = 0; j < Ngrid; j++) {
	    for (i = 0; i < Ngrid; i++) {
		fprintf(stdout,"%d\n",rl[i][j][k]);
		}
	    }
	}
    /*
    ** Give some additonal output
    */
    if (verboselevel >= 1) {
        fprintf(stderr,"Used/resulting values:\n");
        fprintf(stderr,"lmax    : %d\n",lmax);
        fprintf(stderr,"Ngap    : %d\n",Ngap);
        fprintf(stderr,"Nsingle : %d\n",Nsingle);
        }
    exit(0);
    }

void usage(void) {

    fprintf(stderr,"\n");
    fprintf(stderr,"Program converts array in ascii format to array in standard binary format.\n");
    fprintf(stderr,"\n");
    fprintf(stderr,"Please specify the following parameters:\n");
    fprintf(stderr,"\n");
    fprintf(stderr,"-lmax    : maximum refinement level\n");
    fprintf(stderr,"-Ngap    : maximum size of gap that will be filled\n");
    fprintf(stderr,"< <name> : input file in ascii format\n");
    fprintf(stderr,"> <name> : output file in ascii format\n");
    fprintf(stderr,"\n");
    exit(1);
    }
