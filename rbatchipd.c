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

#include <arpa/inet.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <sysexits.h>
#include <string.h>
#include <rpc/rpc.h>
#include "register.h"

int *
register_1_svc(info *msg, struct svc_req *rq)
{
	static int nnodes = 0;
	static int id = -1;
	static int result = 0;
	int rid = -1;
	int i = 0;
	struct sockaddr_in *addr = NULL;
	char *tmp = NULL;
	char *hostfile = NULL;
	FILE *f = NULL;

	if (nnodes == 0) {
		tmp = getenv("AWS_BATCH_JOB_NUM_NODES");
		if (!tmp) {
			errx(EX_SOFTWARE, "unable to retrieve AWS_BATCH_JOB_NUM_NODES");
		}
		nnodes = strtol(tmp, NULL, 10);
	}
#ifdef DEBUG
	printf("AWS_BATCH_JOB_NUM_NODES: %d\n", nnodes);
#endif

	if (id == -1) {
		tmp = getenv("AWS_BATCH_JOB_ID");
		if (!tmp) {
			errx(EX_SOFTWARE, "unable to retrieve AWS_BATCH_JOB_ID");
		}
		id = strtol(tmp, NULL, 10);
	}
#ifdef DEBUG
	printf("server AWS_BATCH_JOB_ID: %d\n", id);
#endif

	rid = msg->jobid;
#ifdef DEBUG
	printf("client AWS_BATCH_JOB_ID: %d\n", rid);
#endif
	if (rid != id) {
		result = 0;
		return &result;
	}

	tmp = getenv("HOME");
	if (!tmp) {
		errx(EX_SOFTWARE, "unable to retrieve HOME");
	}
	i = strlen(tmp) + strlen("hostfile") + 2;
	hostfile = malloc(i);
	snprintf(hostfile, i, "%s/%s", tmp, "hostfile");
#ifdef DEBUG
	printf("Hostfile: %s\n", hostfile);
#endif
	f = fopen(hostfile, "a");
	if (!f) {
		result = 0;
		return &result;
	}
#ifdef DEBUG
	tmp = inet_ntoa((rq->rq_xprt->xp_raddr).sin_addr);
	printf("remote IP: %s\n", tmp);
#endif
	fprintf(f, "%s slots=%d\n", inet_ntoa((rq->rq_xprt->xp_raddr).sin_addr), msg->nslots);
	fclose(f);
	free(hostfile);
	result = 1;
	return &result;
}

