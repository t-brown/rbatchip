/*
 * BSD 3-Clause License
 *
 * Copyright (c) 2023, Timothy Brown
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#define _XOPEN_SOURCE 900

#include <err.h>
#include <stdlib.h>
#include <stdio.h>
#include <sysexits.h>
#include <rpc/rpc.h>
#include <unistd.h>
#include "register.h"
#include "get_env.h"

int
main(int argc, char **argv)
{
	CLIENT *cl = NULL;
	int *result = NULL;
	int nprocs = 0;
	int omp = 0;
	char *server = NULL;
	char *id = NULL;
	info msg = {0};

	/* Get the number of threads */
	omp = strtol(get_env("OMP_NUM_THREADS"), NULL, 10);
	
	/* Get the IP of the main node */
	server = get_env("AWS_BATCH_JOB_MAIN_NODE_PRIVATE_IPV4_ADDRESS");
	id = get_env("AWS_BATCH_JOB_ID");

	cl = clnt_create(server, RBATCHIPPROG, RBATCHIPVERS, "tcp");
	if (!cl) {
		clnt_pcreateerror(server);
		exit(1);
	}

	msg.nslots = sysconf(_SC_NPROCESSORS_ONLN);
	if (omp != 0) {
		msg.nslots = msg.nslots/omp;
	}
	msg.jobid = strtol(id, NULL, 10);
	
	result = register_1(&msg, cl);
	if (!result) {
		clnt_perror(cl, server);
		exit(1);
	}
	if (*result == 0) {
		errx(EX_UNAVAILABLE, "unable to register with %s", server);
	}

	return EXIT_SUCCESS;
}

