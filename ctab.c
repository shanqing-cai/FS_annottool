#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "annot.h"

void free_ctab(CTAB *t) {
  int i0;

  if (t == NULL)
    return;

  if (t->lbl_num) free(t->lbl_num);
  if (t->lbl_name) {
    for (i0 = 0; i0 < t->num_entries; i0++) {
      if (t->lbl_name[i0]) {
	free(t->lbl_name[i0]);
      }	
    }
    free(t->lbl_name);
  }
  if (t->r) free(t->r);
  if (t->g) free(t->g);
  if (t->b) free(t->b);
  if (t->t) free(t->t);
  
  free(t);
}

void read_ctab_from_annot(CTAB *ctab, FILE *af) {
  int tag, ctabversion, maxstruc;
  char *origctabfn; /* SCai: original ctab file name */
  int i0, i1;
  int fnstrlen, len;

  read_int(&tag, af);
  printf("tag = %d\n", tag);

  read_int(&ctabversion, af);
  printf("ctabversion = %d\n", ctabversion);
  
  read_int(&maxstruc, af);
  printf("maxstruc = %d\n", maxstruc);

  read_int(&fnstrlen, af);
  printf("fnstrlen = %d\n", fnstrlen);

  origctabfn = (char *) calloc(fnstrlen + 1, sizeof(char));
  if (origctabfn == NULL) {
    fprintf(stderr, "Failed to allocate memory for origctabfn\n");
    free_ctab(ctab);
    return;
    //goto cleanup;
  }
  free(origctabfn);

  for (i0 = 0; i0 < fnstrlen; i0++) {
    origctabfn[i0] = fgetc(af);
  }
  origctabfn[i0] = '\0';
  printf("origctabfn = \"%s\"\n", origctabfn);

  read_int(&(ctab->num_entries), af);
  printf("num_entries = %d\n", ctab->num_entries);

  /* Start reading look-up table (LUT) */
  ctab->lbl_num = (int *) calloc(ctab->num_entries, sizeof(int));
  if (ctab->lbl_num == NULL) {
    fprintf(stderr, "Failed to allocate memory for ctab.lbl_num\n");
    free_ctab(ctab);
    return;
    //goto cleanup;
  }
  
  ctab->r = (int *) calloc(ctab->num_entries, sizeof(int));
  if (ctab->r == NULL) {
    fprintf(stderr, "Failed to allocate memory for ctab.r\n");
    free_ctab(ctab);
    return;
    //goto cleanup;
  }
  
  ctab->g = (int *) calloc(ctab->num_entries, sizeof(int));
  if (ctab->g == NULL) {
    fprintf(stderr, "Failed to allocate memory for ctab.g\n");
    free_ctab(ctab);
    return;
    //goto cleanup;
  }

  ctab->b = (int *) calloc(ctab->num_entries, sizeof(int));
  if (ctab->b == NULL) {
    fprintf(stderr, "Failed to allocate memory for ctab.b\n");
    free_ctab(ctab);
    return;
    //goto cleanup;
  }
  
  ctab->t = (int *) calloc(ctab->num_entries, sizeof(int));
  if (ctab->t == NULL) {
    fprintf(stderr, "Failed to allocate memory for ctab.t\n");
    free_ctab(ctab);
    return;
    //goto cleanup;
  }

  ctab->lbl_name = (char **) calloc(ctab->num_entries, sizeof(char *));
  if (ctab->lbl_name == NULL) {
    fprintf(stderr, "Failed to allocate memory for ctab.lbl_name\n");
    free_ctab(ctab);
    return;
    //goto cleanup;
  }

  printf("\nColor table: \n");
  for (i0 = 0; i0 < ctab->num_entries; ++i0) {
    read_int(ctab->lbl_num + i0, af);

    read_int(&len, af);
    ctab->lbl_name[i0] = (char *) calloc(len + 1, sizeof(char));
    if (ctab->lbl_name[i0] == NULL) {
      fprintf(stderr, "Failed to allocate memory for ctab_lbl_name[%d]\n", i0);
      free_ctab(ctab);
      return;
      //goto cleanup;
    }
    for (i1 = 0; i1 < len; ++i1) {
      ctab->lbl_name[i0][i1] = fgetc(af);
    }
    ctab->lbl_name[i0][i1] = '\0';

    read_int(ctab->r + i0, af);
    read_int(ctab->g + i0, af);
    read_int(ctab->b + i0, af);
    read_int(ctab->t + i0, af);
  }
}

void print_ctab(CTAB *t) {
  int i0;
  
  for (i0 = 0; i0 < t->num_entries; i0++) {
    printf("%d\t%s\t%d\t%d\t%d\t%d\n", 
	   t->lbl_num[i0], t->lbl_name[i0], 
	   t->r[i0], t->g[i0], t->b[i0], t->t[i0]);
  }
}

void read_ctab_from_ctab(CTAB *ctab, FILE *ctabf) {
  char c1;
  int nlines;
  char roi_name[64];
  int roi_num;
  int r, g, b, transp;
  int len, i0;

  // Count the number of lines
  nlines = 0;
  c1 = '\0';
  while (c1 != EOF) {
    c1 = fgetc(ctabf);
    if (c1 == '\n' || c1 == '\r')
      nlines++;
  }
  //printf("nlines = %d\n", nlines);
  
  ctab->num_entries = nlines;
  ctab->lbl_num = (int *) calloc(ctab->num_entries, sizeof(int));
  if (ctab->lbl_num == NULL) {
    fprintf(stderr, "Failed to allocate memory for ctab.lbl_num\n");
    free_ctab(ctab);
    return;
  }
  
  ctab->r = (int *) calloc(ctab->num_entries, sizeof(int));
  if (ctab->r == NULL) {
    fprintf(stderr, "Failed to allocate memory for ctab.r\n");
    free_ctab(ctab);
    return;
  }
  
  ctab->g = (int *) calloc(ctab->num_entries, sizeof(int));
  if (ctab->g == NULL) {
    fprintf(stderr, "Failed to allocate memory for ctab.g\n");
    free_ctab(ctab);
    return;
  }

  ctab->b = (int *) calloc(ctab->num_entries, sizeof(int));
  if (ctab->b == NULL) {
    fprintf(stderr, "Failed to allocate memory for ctab.b\n");
    free_ctab(ctab);
    return;
  }
  
  ctab->t = (int *) calloc(ctab->num_entries, sizeof(int));
  if (ctab->t == NULL) {
    fprintf(stderr, "Failed to allocate memory for ctab.t\n");
    free_ctab(ctab);
    return;
  }

  ctab->lbl_name = (char **) calloc(ctab->num_entries, sizeof(char *));
  if (ctab->lbl_name == NULL) {
    fprintf(stderr, "Failed to allocate memory for ctab.lbl_name\n");
    free_ctab(ctab);
    return;
  }
  
  rewind(ctabf);

  for (i0 = 0; i0 < ctab->num_entries; ++i0) {
    fscanf(ctabf, "%d", &roi_num);
    ctab->lbl_num[i0] = roi_num;
    //printf("ctab->lbl_num[%d] = %d\n", i0, ctab->lbl_num[i0]);

    fscanf(ctabf, "%s", roi_name);
    len = strlen(roi_name);
    //printf("len = %d\n", len);
    ctab->lbl_name[i0] = (char *) calloc(len + 1, sizeof(char));
    if (ctab->lbl_name[i0] == NULL) {
      fprintf(stderr, "Failed to allocate memory for lbl_name[%d]", i0);
      free_ctab(ctab);
      return;
    }
    strcpy(ctab->lbl_name[i0], roi_name);
    //printf("ctab->lbl_name[%d] = %s\n", i0, ctab->lbl_name[i0]);
  
    fscanf(ctabf, "%d", &r);
    if (r < 0 || r > 255) {
      fprintf(stderr, 
	      "The format of the color table file appears to be wrong.\n");
      free_ctab(ctab);
      return;
    }
    ctab->r[i0] = r;
    //printf("ctab->r[%d] = %d\n", i0, ctab->r[i0]);

    fscanf(ctabf, "%d", &g);
    if (g < 0 || g > 255) {
      fprintf(stderr, 
	      "The format of the color table file appears to be wrong.\n");
      free_ctab(ctab);
      return;
    }
    ctab->g[i0] = g;
    //printf("ctab->g[%d] = %d\n", i0, ctab->g[i0]);

    fscanf(ctabf, "%d", &b);
    if (b < 0 || b > 255) {
      fprintf(stderr, 
	      "The format of the color table file appears to be wrong.\n");
      free_ctab(ctab);
      return;
    }
    ctab->b[i0] = b;
    //printf("ctab->b[%d] = %d\n", i0, ctab->b[i0]);

    fscanf(ctabf, "%d", &transp);
    if (transp < 0 || transp > 255) {
      fprintf(stderr, 
	      "The format of the color table file appears to be wrong.\n");
      free_ctab(ctab);
      return;
    }
    ctab->t[i0] = transp;
    //printf("ctab->t[%d] = %d\n", i0, ctab->t[i0]);
  }

}

int get_struc_num(CTAB *ctab, char roi_name[]) {
  int i0, struc_num;
  int bFound = 0;
  
  for (i0 = 0; i0 < ctab->num_entries; i0++) {
    if (!strcmp(ctab->lbl_name[i0], roi_name)) {
      bFound = 1;
      struc_num = ctab->r[i0] + ctab->g[i0] * 256 + ctab->b[i0] * 256 * 256;
      break;
    }
  }

  if (bFound == 0)
    return -1;
  else
    return struc_num;
}
