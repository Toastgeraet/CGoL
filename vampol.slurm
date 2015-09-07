#!/bin/sh
	
# Time limit is one minute.
# See "man sbatch" for other time formats.
#SBATCH --time=5
# Run 4 tasks on 2 nodes.
#SBATCH -N 1 -n 2
# Output goes to job.out, error messages to job.err.
#SBATCH --error=job_vampol.err --output=job_vampol.out
	
srun hostname
mpiexec ./vampol inputfiles/world0.txt -x 100 -y 100 -z 100 -g 100
