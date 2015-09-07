#!/bin/sh
	
# Time limit is one minute.
# See "man sbatch" for other time formats.
#SBATCH --time=5
# Run on 1 nodes.
#SBATCH -N 1
# Output goes to job.out, error messages to job.err.
#SBATCH --error=job_ogol.err --output=job_ogol.out

module load profile
module load analysis
module load oprofile

srun hostname
srun operf ./singol inputfiles/world0.txt -x 100 -y 100 -z 100 -g 100
