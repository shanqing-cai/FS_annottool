#!/usr/bin/python

import os
import sys
import glob
import argparse

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Generate a new ctab based on a directory with labels")
    parser.add_argument("in_ctab", type=str, help="Input ctab file name")
    parser.add_argument("lbl_dir", type=str, help="Directory with all label files")
    parser.add_argument("out_ctab", type=str, help="Output ctab file name")

    if len(sys.argv) == 1:
        parser.print_help()
        sys.exit(0)

    args = parser.parse_args()
    in_ctab = args.in_ctab
    lbl_dir = args.lbl_dir
    out_ctab = args.out_ctab

    if not os.path.isfile(in_ctab):
        raise Exception, "Cannot find input ctab: %s"%in_ctab
    if not os.path.isdir(lbl_dir):
        raise Exception, "Cannot find labels directory: %s"%lbl_dir

    dlbls = glob.glob(os.path.join(lbl_dir, "?h.*.label"))
    if len(dlbls) == 0:
        print("INFO: label directory is empty.")
        sys.exit(0)
    dlbls.sort()
    
    rois = []
    for fn in dlbls:
        (fpath, fnn) = os.path.split(fn)
        t_roi = fnn.split('.')[1]
        rois.append(t_roi)

    # ------ Read from the input ctab ------ #
    inctabf = open(in_ctab, 'r')
    intxt = inctabf.read().split('\n')
    inctabf.close()
    
    outtxt = ""
    roicnt = 0
    for t_line in intxt:
        if len(t_line) == 0:
            continue

        t_line = t_line.replace('\t', ' ')
        t_items = t_line.split(' ')
        while t_items.count('') > 0:
            t_items.remove('')

        t_roi = t_items[1]
        t_clr_r = int(t_items[2])
        t_clr_g = int(t_items[3])
        t_clr_b = int(t_items[4])
        t_clr_a = int(t_items[5])
        if rois.count(t_roi) == 1:
            t_line_new = "%d\t%s\t%d\t%d\t%d\t%d"\
                         %(roicnt, t_roi, t_clr_r, t_clr_g, t_clr_b, t_clr_a)
            outtxt += t_line_new + '\n'
            rois.remove(t_roi)
            roicnt += 1

    outf = open(out_ctab, 'w')
    outf.write(outtxt)
    outf.close()
