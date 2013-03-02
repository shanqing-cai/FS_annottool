# FS_annottool
A set of tools for manipulation of FreeSurfer surface label and annotation files.
Author: Shanqing Cai (shanqing.cai@gmail.com)
Date: 09/13/2012

--- Main programs ---
maplabels.py
formatctab.py
label2annot
readannot


--- The following description details the procedures involved in transferring the annotation from one brain to another brain. These two brains should be fairly similar, e.g., from two scans of the same adult subject not separated by two long. ---


Preparation: compile label2annot
cd to the source directory
make

Add path to the source directory, which contains the python scripts and the binary executable "label2annot". Alternatively, you can copy them to a centralized location for executables, such as /usr/local/bin.

First, use mri_annotation2label to convert the original annotation file to a directory of labels.

cd /users/cais/STUT/FSDATA/S01_nofix/label

mri_annotation2label --subject S01_nofix --annotation aparc12 --hemi rh --outdir ./aparc12_rh


Second, use maplabels.py to transfer the labels from the source subject to the target subject

maplabels.py S01_nofix /users/cais/STUT/FSDATA/S01_nofix/label/aparc12_rh rh S01 /users/cais/STUT/FSDATA/S01/label/aparc12_rh

This operation may take a while.


Then, use formatctab.py to generate a new color table based on the labels that are in the new label directory. This requires an existing ctab (possibly the master ctab) as an input. 

formatctab.py /users/cais/STUT/slFRS17.ctab /users/cais/STUT/FSDATA/S01/label/aparc12_rh /users/cais/STUT/slFRS17_short.ctab

The new ctab will keep only the ROIs (labels) that are in the label directory. It will preserve the color values (R, G, B, T) of the old ctab, but will re-arrange the indices.

Finally, use the "label2annot" program to create the new annot file from

label2annot /users/cais/STUT/FSDATA/S01/label/aparc12_rh /users/cais/STUT/slFRS17_short.ctab rh.aparc12_new.annot

The input arguments are, in order: the label directory generated by maplabels.py, the color table generated by formatctab.py, and the file name of the output new annot. Note that in this case, the new annot file will be at the current working directory.