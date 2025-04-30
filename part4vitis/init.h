#pragma once
#include "constants.h"
#include "xaxicdma.h"
#include "xparameters.h"
#include "xgpio.h"

int init_system(XGpio* Gpio1);
int init_CDMA(XAxiCdma_Config* cdma_config);