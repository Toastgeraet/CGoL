#!/bin/sh
	
# Time limit is one minute.
# See "man sbatch" for other time formats.
#SBATCH --time=5
# Run 4 tasks on 2 nodes.
#SBATCH -N 1
# Output goes to job.out, error messages to job.err.
#SBATCH --error=job_pargol.err --output=job_pargol.out
	
srun hostname
mpiexec ./pargol inputfiles/world0.txt -x 100 -y 100 -z 100 -g 100
