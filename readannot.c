#include <stdio.h>
#include <stdlib.h>

#include "annot.h"

int main(int argc, char *argv[]) {
  char *annotfn;
  
  FILE *af;
  int vtxidx;
  int vtxct;
  int *vno, *label;
  
  //int num_entries;
  //int *ctab_lbl_num, *red, *green, *blue, *transp;
  //char **ctab_lbl_name;
  int lbl, vtxr, vtxg, vtxb;
  CTAB *ctab;

  int i0;

  if (argc < 3) {
    printf("Usage: readannot annotfn vtxidx\n");
    exit(0);
  }

  annotfn = argv[1];
  printf("annotfn = %s\n", annotfn);

  vtxidx = atoi(argv[2]);
  printf("vtxidx = %d\n", vtxidx);

  if ((af = fopen(annotfn, "r")) == NULL) {
    fprintf(stderr, "Failed to open annot file: %s\n", annotfn);
    exit(0);
  }

  read_int(&vtxct, af);
  printf("vtxct = %d\n", vtxct);

  vno = (int *) calloc(vtxct, sizeof(int));
  if (vno == NULL) {
    fprintf(stderr, "Failed to allocate memory for vno\n");
    goto cleanup;
  }

  label = (int *) calloc(vtxct, sizeof(int));
  if (label == NULL) {
    fprintf(stderr, "Failed to allocted memory for label");
    goto cleanup;
  }
  
  for (i0 = 0; i0 < vtxct; i0++) {
    read_int(vno + i0, af);
    read_int(label + i0, af);
  }

  //printf("vno[20730] = %d\n", vno[20730]);
  //printf("label[20730] = %d\n", label[20730]);
  //printf("vno[vtxct - 1] = %d\n", vno[vtxct - 1]);
  //printf("label[vtxct - 1] = %d\n", label[vtxct = 1]);
  ctab = (CTAB *) malloc(sizeof(CTAB));
  
  read_ctab_from_annot(ctab, af);
  if (ctab == NULL) {
    fprintf(stderr, "Failed to read color table from annot file: %s", annotfn);
    goto cleanup;
  }
  print_ctab(ctab);

  //printf("size = %d\n", sizeof(int));
  printf("vno[%d] = %d\n", vtxidx, vno[vtxidx]);
  printf("label[%d] = %d\n", vtxidx, label[vtxidx]);

  lbl = label[vtxidx];
  vtxb = lbl / (256 * 256);
  lbl -= vtxb * (256 * 256);
  vtxg = lbl / 256;
  lbl -= vtxg * 256;
  vtxr = lbl;
  printf("vtxr = %d; vtxg = %d; vtxb = %d\n", vtxr, vtxg, vtxb);

  /* cleanup */
 cleanup:
  if (vno)
    free(vno);
  if (label)
    free(label);

  free_ctab(ctab);

  if (af)
    fclose(af);

  return 0;
}
