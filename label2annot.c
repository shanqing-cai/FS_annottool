#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

#include "annot.h"

int main(int argc, char *argv[]) {
  char *indir, *ctabfn, *annotout;
  FILE *ctabf, *annotf;
  FILE *lblf;
  DIR *dp;
  char fpath[512];
  struct dirent *ep;
  int nvtx, tot_nvtx;
  int slen;
  CTAB *ctab;
  char t_roi_name[32];
  int *vno, *label, max_vno;
  int *f_vno, *f_label, ovwct; // f_ stands for final (after sorting and overwriting)
  int vctr;
  int t_struc_num;
  int i0;
  //int *t_vno, *t_label;

  if (argc != 4) {
    printf("Usage: label2annot indir ctabfn annotout\n");
    exit(0);
  }

  indir = argv[1];
  ctabfn = argv[2];
  annotout = argv[3];

  if ((ctabf = fopen(ctabfn, "r")) == NULL) {
    fprintf(stderr, "Failed to open color table file: %s\n", ctabfn);
    goto cleanup;
  }

  /* Read color table from the ctab */
  ctab = (CTAB *) malloc(sizeof(CTAB));
  read_ctab_from_ctab(ctab, ctabf);
  if (ctab == NULL) {
    fprintf(stderr, "Failed to color table from ctab file: %s", ctabfn);
    goto cleanup;
  }
  print_ctab(ctab);
  printf("\n");

  /* Get the list of files that are in indir */
  dp = opendir(indir);
  if (dp == NULL) {
    fprintf(stderr, "Failed to access input directory: %s\n", indir);
    goto cleanup;
  }
  
  tot_nvtx = 0;
  while ((ep = readdir(dp))) {
    if (strlen(ep->d_name) < 10)
      continue;
    slen = strlen(ep->d_name);
    if (!((ep->d_name[0] == 'l' || ep->d_name[0] == 'r') 
	  && (ep->d_name[1] == 'h') && (ep->d_name[2] == '.') 
	  && (ep->d_name[slen - 5] == 'l') && (ep->d_name[slen - 4] == 'a')
	  && (ep->d_name[slen - 3] == 'b') && (ep->d_name[slen - 2] == 'e')
	  && (ep->d_name[slen - 1] == 'l'))) {
	continue;
    }
    
    strcpy(fpath, indir);
    strcat(fpath, "/");
    strcat(fpath, ep->d_name);
    if ((lblf = fopen(fpath, "r")) == NULL) {
      fprintf(stderr, "Cannot open label file: %s", fpath);
      goto cleanup;
    }
    
    nvtx = lblf_nvtx(lblf);
    tot_nvtx += nvtx;

    fclose(lblf);

    //printf("fn = %s; nvtx = %d; tot_nvtx = %d\n", 
    //       fpath, nvtx, tot_nvtx);
    
  }

  //printf("\ntot_ntvx = %d\n", tot_nvtx);

  /* Convert label data to annot data */
  vno = (int *) calloc(tot_nvtx, sizeof(int));
  if (vno == NULL) {
    fprintf(stderr, "Failed to allocated memory for vno\n");
    goto cleanup;
  }
  label = (int *) calloc(tot_nvtx, sizeof(int));
  if (label == NULL) {
    fprintf(stderr, "Failed to allocated memory for label\n");
    goto cleanup;
  }

  rewinddir(dp);
  vctr = 0;

  while ((ep = readdir(dp))) {
    if (strlen(ep->d_name) < 10)
      continue;
    slen = strlen(ep->d_name);
    if (!((ep->d_name[0] == 'l' || ep->d_name[0] == 'r') 
	  && (ep->d_name[1] == 'h') && (ep->d_name[2] == '.') 
	  && (ep->d_name[slen - 5] == 'l') && (ep->d_name[slen - 4] == 'a')
	  && (ep->d_name[slen - 3] == 'b') && (ep->d_name[slen - 2] == 'e')
	  && (ep->d_name[slen - 1] == 'l'))) {
	continue;
    }

    
    strncpy(t_roi_name, ep->d_name + 3, slen - 3 - 6);
    t_roi_name[slen - 3 - 6] = '\0';
    
    strcpy(fpath, indir);
    strcat(fpath, "/");
    strcat(fpath, ep->d_name);
    if ((lblf = fopen(fpath, "r")) == NULL) {
      fprintf(stderr, "Cannot open label file: %s", fpath);
      goto cleanup;
    }
    printf("Reading from label file: %s...\n", fpath);

    // Get the struc (label) number
    t_struc_num = get_struc_num(ctab, t_roi_name);
    printf("ROI name = %s; struc num = %d\n\n", t_roi_name, t_struc_num);
    
    nvtx = lblf_nvtx(lblf);
    get_lblf_vno(lblf, vno + vctr, nvtx);

    for (i0 = 0; i0 < nvtx; ++i0) {
      label[vctr + i0] = t_struc_num;
    }

    vctr += nvtx;
    fclose(lblf);


  }

  //printf("\ntot_nvtx = %d; vctr = %d\n", tot_nvtx, vctr);

  /* Sort the data by vno, deal with overwriting */
  max_vno = -1;
  for (i0 = 0; i0 < tot_nvtx; ++i0)
    if (vno[i0] > max_vno)
      max_vno = vno[i0];
  //printf("max_vno = %d\n", max_vno);
  
  if ((f_vno = (int *) calloc(max_vno + 1, sizeof(int))) == NULL) {
    fprintf(stderr, "Failed to allocate memory for f_vno\n");
    goto cleanup;
  }
  if ((f_label = (int *) calloc(max_vno + 1, sizeof(int))) == NULL) {
    fprintf(stderr, "Failed to allocate memory for f_label\n");
    goto cleanup;
  }

  // Initialize all elements of f_label to -1, so that we can keep track of overwrites
  for (i0 = 0; i0 < max_vno + 1; ++i0) {
    f_vno[i0] = i0;
    f_label[i0] = -1;
  }
  
  ovwct = 0;
  for (i0 = 0; i0 < tot_nvtx; ++i0) {
    if (f_label[vno[i0]] != -1)
      ovwct += 1;

    f_label[vno[i0]] = label[i0];
  }
  printf("Number of overwrites = %d (%.2f%%)\n", 
	 ovwct, (float)ovwct / (float)tot_nvtx * 100.0);

  /* Write to annot file */
  if ((annotf = fopen(annotout, "w")) == NULL) {
    fprintf(stderr, "Failed to open output annot file for writing: %s",
	    annotout);
  }
  
  printf("Writing to output annot file: %s...\n", annotout);
  write_annot(annotf, max_vno + 1, f_vno, f_label, ctabfn, ctab);
  
  //i0 = 3;
  //printf("byterev_int(i0) = %d\n", byterev_int(i0));
  //printf("byterev(byterev_int(i0)) = %d\n", byterev_int(byterev_int(i0)));

 cleanup:
  if (ctabf) fclose(ctabf);
  if (annotf) fclose(annotf);
  if (dp) closedir(dp);
  //if (lblf) fclose(lblf);
  if (ctab) free_ctab(ctab);
  if (vno) free(vno);
  if (label) free(label);
  if (f_vno) free(f_vno);
  if (f_label) free(f_label);

  return 0;
}
