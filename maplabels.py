#!/usr/bin/python

import os
import sys
import argparse
import glob

from scai_utils import saydo

FSDATA_DIR = "/users/cais/STUT/FSDATA"

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Map all labels in one directory to a target directory using mri_label2label")
    parser.add_argument("src_sID", type=str,  help="Source subject ID")
    parser.add_argument("src_dir", type=str, help="Source directory with in the label sub-directory of the subject")
    parser.add_argument("hemi", type=str, help="Hemisphere")
    parser.add_argument("trg_sID", type=str, help="Target subject ID")
    parser.add_argument("trg_dir", type=str, help="Destination path for writing the new label files");
    
    if len(sys.argv) == 1:
        parser.print_help()
        sys.exit(0)

    args = parser.parse_args()
    src_sID = args.src_sID
    src_dir = args.src_dir
    hemi = args.hemi
    trg_sID = args.trg_sID
    trg_dir = args.trg_dir

    if not os.path.isdir(trg_dir):
        saydo("mkdir %s"%trg_dir)

    # Determine the set of ROIs
    cwd0 = os.getcwd()
    srcdir = os.path.join(FSDATA_DIR, src_sID, "label", src_dir)
    if not os.path.isdir(srcdir):
        raise Exception, "Cannot find source directory at location: %s"%(srcdir)

    d0 = glob.glob(os.path.join(srcdir, "%s.*.label"%(hemi)))
    if len(d0) == 0:
        raise Exception, "Found no files in source directory: %s"%(srcdir)

    d0.sort()
    rois = []
    for fn in d0:
        [fpath, fnn] = os.path.split(fn)
        rois.append(fnn.split('.')[1])
    
    nrois = len(rois)
    print("Found %d ROIS in source directory %s"%(nrois, srcdir))

    os.chdir(srcdir)
    for (i0, roi) in enumerate(rois):
        srclblfn = "%s.%s.label"%(hemi, roi)
        trglblfn = os.path.join(trg_dir, srclblfn)
        mapcmd = "mri_label2label --srclabel %s --srcsubject %s "%(srclblfn, src_sID) + \
                 "--trglabel %s --trgsubject %s "%(trglblfn, trg_sID) + \
                 "--regmethod surface --hemi %s"%(hemi)
        saydo(mapcmd)

    os.chdir(cwd0)
