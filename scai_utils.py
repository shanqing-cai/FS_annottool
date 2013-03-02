import os
import sys

def saydo(cmd, echo=True):
    if echo:
        print(cmd + '\n')

    os.system(cmd)

def qsubmit(cmd, queue, jobname):
    os.system('ezsub -c "%s" -q %s -n %s'%(cmd, queue, jobname))

def read_ctab(ctabfn):
    ctabf = open(ctabfn, 'r')
    ctxt = ctabf.read().split('\n')
    ctabf.close()
    
    roi_nums = []
    roi_names = []
    for clin in ctxt:
        if len(clin) == 0:
            continue
        clin = clin.split(' ')
        while clin.count('') > 0:
            clin.remove('')
            
        if clin[1] == 'unknown' or clin[1] == 'bankssts' \
           or clin[1] == "corpuscallosum" or clin[1] == "Unknown" \
           or clin[1].startswith("None"):
            continue
        
        roi_nums.append(int(clin[0]))
        roi_names.append(clin[1])

    return (roi_nums, roi_names)
