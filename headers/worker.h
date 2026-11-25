#pragma once
#ifndef WORKER_H
#define WORKER_H

#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>

#include "objects.h"
#include "handler.h"
#include "utils.h"

void *cnc_worker(void *);

#endif // WORKER_H