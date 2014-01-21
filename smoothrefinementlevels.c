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

	for (i = isel+1; i <= Ngrid+isel; i++) {
		l = put_index_in_box(Ngrid,i);
		if (rl[l][jsel][ksel] == level) {
			*inext = l;
			break;
			}
		}
	for (j = jsel+1; j <= Ngrid+jsel; j++) {
		l = put_index_in_box(Ngrid,j);
		if (rl[isel][l][ksel] == level) {
			*jnext = l;
			break;
			}
		}
	for (k = ksel+1; k <= Ngrid+ksel; k++) {
		l = put_index_in_box(Ngrid,k);
		if (rl[isel][jsel][l] == level) {
			*knext = l;
			break;
			}
		}
	}

int main(int argc, char **argv) {

	int i,j,k,l,m,n,loop;
	int inext,jnext,knext;
	int ltemp,ntemp,mtemp;
	int idummy;
	int Ntot,Ngrid,Lcurrent;
	int Lmax,Ngap,Ngapsingle,Nbuffer;
	int Nsingle;
	int Lgap;
	int verboselevel;
	int ***rl = NULL;
	double meanlevel;

	verboselevel = 0;
	Lmax = 0;
	Ngap = 5;
	Ngapsingle = 2;
	Nbuffer = 3;
	/* Nsmooth = 1; */
	/* Nsmoothaccept = 1; */
	Lgap = 2;
	inext = 0;
	jnext = 0;
	knext = 0;

	i = 1;
	while (i < argc) {
		if (strcmp(argv[i],"-Lmax") == 0) {
			i++;
			if (i >= argc) {
				usage();
				}
			Lmax = atoi(argv[i]);
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
		else if (strcmp(argv[i],"-Ngapsingle") == 0) {
			i++;
			if (i >= argc) {
				usage();
				}
			Ngapsingle = atoi(argv[i]);
			i++;
			}
		else if (strcmp(argv[i],"-Nbuffer") == 0) {
			i++;
			if (i >= argc) {
				usage();
				}
			Nbuffer = atoi(argv[i]);
			i++;
			}
		else if (strcmp(argv[i],"-Lgap") == 0) {
			i++;
			if (i >= argc) {
				usage();
				}
			Lgap = atoi(argv[i]);
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
	assert(Lmax >=0);
	/* Nneigh = pow(2.0*Nsmooth+1.0,3.0)-1.0; */
	/* fsmooth = (Nneigh-Nsmoothaccept)/((double) Nneigh); */

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
				}
			}
		}

	/*
	** Go through particles and find gaps in high-resolution region
	*/

	for (loop = 0; loop < Lgap; loop++) {
		for (k = 0; k < Ngrid; k++) {
			for (j = 0; j < Ngrid; j++) {
				for (i = 0; i < Ngrid; i++) {
					if (rl[i][j][k] != Lmax) continue;
					inext = jnext = knext = -1;
					find_next(rl,Ngrid,Lmax,i,j,k,&inext,&jnext,&knext);
					assert(inext >= 0 && inext < Ngrid);
					assert(jnext >= 0 && jnext < Ngrid);
					assert(knext >= 0 && knext < Ngrid);
					if (inext <= i) inext += Ngrid;
					if (jnext <= j) jnext += Ngrid;
					if (knext <= k) knext += Ngrid;
					if (inext-i <= Ngap+1) {
						for (l = i+1; l < inext; l++) {
							m = put_index_in_box(Ngrid,l);
							rl[m][j][k] = Lmax;
							}
						}
					if (jnext-j <= Ngap+1) {
						for (l = j+1; l < jnext; l++) {
							m = put_index_in_box(Ngrid,l);
							rl[i][m][k] = Lmax;
							}
						}
					if (knext-k <= Ngap+1) {
						for (l = k+1; l < knext; l++) {
							m = put_index_in_box(Ngrid,l);
							rl[i][j][m] = Lmax;
							}
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
				if (rl[i][j][k] != Lmax) continue;
				meanlevel = 0;
				for (l = i-Ngapsingle; l <= i+Ngapsingle; l++) {
					for (m = j-Ngapsingle; m <= j+Ngapsingle; m++) {
						for (n = k-Ngapsingle; n <= k+Ngapsingle; n++) {
							ltemp = put_index_in_box(Ngrid,l);
							mtemp = put_index_in_box(Ngrid,m);
							ntemp = put_index_in_box(Ngrid,n);
							if (ltemp == i && mtemp == j && ntemp == k) continue;
							if (rl[ltemp][mtemp][ntemp] == Lmax) {
								goto outloop;
								}
							else {
								meanlevel += rl[ltemp][mtemp][ntemp];
								}
							}
						}
					}
				meanlevel /= pow(2.0*Ngapsingle+1.0,3.0)-1.0;
				Nsingle++;
				rl[i][j][k] = (int) (meanlevel+0.5);
			outloop:
				continue;
				}
			}
		}

	/*
	** Add buffer regions around high-resolution region
	*/

	for (Lcurrent = Lmax-1; Lcurrent > 0; Lcurrent--) {
		for (k = 0; k < Ngrid; k++) {
			for (j = 0; j < Ngrid; j++) {
				for (i = 0; i < Ngrid; i++) {

					/* 
					** Check for forward step (increasing index)
					*/

					l = put_index_in_box(Ngrid,i-1);
					if (rl[i][j][k] == Lcurrent+1 && rl[l][j][k] <= Lcurrent) {
						for (l = i-Nbuffer; l < i; l++) {
							for (m = j-Nbuffer; m <= j+Nbuffer; m++) {
								for (n = k-Nbuffer; n <= k+Nbuffer; n++) {
									ltemp = put_index_in_box(Ngrid,l);
									mtemp = put_index_in_box(Ngrid,m);
									ntemp = put_index_in_box(Ngrid,n);
									if (rl[ltemp][mtemp][ntemp] < Lcurrent) rl[ltemp][mtemp][ntemp] = Lcurrent;
									}
								}
							}
						}
					m = put_index_in_box(Ngrid,j-1);
					if (rl[i][j][k] == Lcurrent+1 && rl[i][m][k] <= Lcurrent) {
						for (m = j-Nbuffer; m < j; m++) {
							for (l = i-Nbuffer; l < i+Nbuffer; l++) {
								for (n = k-Nbuffer; n <= k+Nbuffer; n++) {
									ltemp = put_index_in_box(Ngrid,l);
									mtemp = put_index_in_box(Ngrid,m);
									ntemp = put_index_in_box(Ngrid,n);
									if (rl[ltemp][mtemp][ntemp] < Lcurrent) rl[ltemp][mtemp][ntemp] = Lcurrent;
									}
								}
							}
						}
					n = put_index_in_box(Ngrid,k-1);
					if (rl[i][j][k] == Lcurrent+1 && rl[i][j][n] <= Lcurrent) {
						for (n = k-Nbuffer; n < k; n++) {
							for (l = i-Nbuffer; l < i+Nbuffer; l++) {
								for (m = j-Nbuffer; m <= j+Nbuffer; m++) {
									ltemp = put_index_in_box(Ngrid,l);
									mtemp = put_index_in_box(Ngrid,m);
									ntemp = put_index_in_box(Ngrid,n);
									if (rl[ltemp][mtemp][ntemp] < Lcurrent) rl[ltemp][mtemp][ntemp] = Lcurrent;
									}
								}
							}
						}

					/* 
					** Check for backward step (decreasing index)
					*/

					l = put_index_in_box(Ngrid,i+1);
					if (rl[i][j][k] == Lcurrent+1 && rl[l][j][k] <= Lcurrent) {
						for (l = i+Nbuffer; l > i; l--) {
							for (m = j-Nbuffer; m <= j+Nbuffer; m++) {
								for (n = k-Nbuffer; n <= k+Nbuffer; n++) {
									ltemp = put_index_in_box(Ngrid,l);
									mtemp = put_index_in_box(Ngrid,m);
									ntemp = put_index_in_box(Ngrid,n);
									if (rl[ltemp][mtemp][ntemp] < Lcurrent) rl[ltemp][mtemp][ntemp] = Lcurrent;
									}
								}
							}
						}
					m = put_index_in_box(Ngrid,j+1);
					if (rl[i][j][k] == Lcurrent+1 && rl[i][m][k] <= Lcurrent) {
						for (m = j+Nbuffer; m > j; m--) {
							for (l = i-Nbuffer; l < i+Nbuffer; l++) {
								for (n = k-Nbuffer; n <= k+Nbuffer; n++) {
									ltemp = put_index_in_box(Ngrid,l);
									mtemp = put_index_in_box(Ngrid,m);
									ntemp = put_index_in_box(Ngrid,n);
									if (rl[ltemp][mtemp][ntemp] < Lcurrent) rl[ltemp][mtemp][ntemp] = Lcurrent;
									}
								}
							}
						}
					n = put_index_in_box(Ngrid,k+1);
					if (rl[i][j][k] == Lcurrent+1 && rl[i][j][n] <= Lcurrent) {
						for (n = k+Nbuffer; n > k; n--) {
							for (l = i-Nbuffer; l < i+Nbuffer; l++) {
								for (m = j-Nbuffer; m <= j+Nbuffer; m++) {
									ltemp = put_index_in_box(Ngrid,l);
									mtemp = put_index_in_box(Ngrid,m);
									ntemp = put_index_in_box(Ngrid,n);
									if (rl[ltemp][mtemp][ntemp] < Lcurrent) rl[ltemp][mtemp][ntemp] = Lcurrent;
									}
								}
							}
						}

					}
				}
			}

		} /* Level loop */

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
		fprintf(stderr,"Lmax	   : %d\n",Lmax);
		fprintf(stderr,"Ngap	   : %d\n",Ngap);
		fprintf(stderr,"Ngapsingle : %d\n",Ngapsingle);
		fprintf(stderr,"Nbuffer	   : %d\n",Nbuffer);
		fprintf(stderr,"Lgap	   : %d\n",Lgap);
		fprintf(stderr,"Nsingle	   : %d\n",Nsingle);
		}

	exit(0);
	}

void usage(void) {

	fprintf(stderr,"\n");
	fprintf(stderr,"Program converts array in ascii format to array in standard binary format.\n");
	fprintf(stderr,"\n");
	fprintf(stderr,"Please specify the following parameters:\n");
	fprintf(stderr,"\n");
	fprintf(stderr,"-Lmax		: maximum refinement level\n");
	fprintf(stderr,"-Ngap		: maximum size of gap that will be filled (default: 5)\n");
	fprintf(stderr,"-Ngapsingle : size of gap around particles to identify singles (default: 2)\n");
	fprintf(stderr,"-Nbuffer	: size of each buffer zone around high-resolution particles (default: 3)\n");
	fprintf(stderr,"-Lgap		: number of loops over gap removal process (default: 2)\n");
	fprintf(stderr,"-v			: get a bit more details\n");
	fprintf(stderr,"< <name>	: input file in ascii format\n");
	fprintf(stderr,"> <name>	: output file in ascii format\n");
	fprintf(stderr,"\n");
	exit(1);
	}
