#include "init.h"
#include "xaxicdma.h"
#include <xaxicdma.h>
#include <xparameters.h>
#include <xstatus.h>

int init_system(XGpio* Gpio1)
{
    int Status = XST_SUCCESS;

	Status = XGpio_Initialize(Gpio1, GPIO_ONE);
	if (Status != XST_SUCCESS) {
		xil_printf("Gpio Initialization Failed\r\n");
		return XST_FAILURE;
	}
    
    Status = XGpio_SelfTest(Gpio1);
	if (Status != XST_SUCCESS) {
		xil_printf("Gpio self-test Failed\r\n");
		return XST_FAILURE;
	}

    if(Status == XST_FAILURE) {
        print("System init failed\r\n");
        while(1);
    }

    return Status;
}

int init_CDMA(XAxiCdma* inst_ptr) {

    XAxiCdma_Config* config_ptr = NULL;
    int status = XST_FAILURE;
    int select_keyhole = XST_FAILURE;

#if 0 // Already done in CfgInitialize

    XAxiCdma_Reset(inst_ptr);

    while (!XAxiCdma_ResetIsDone(inst_ptr));

#endif

    config_ptr = XAxiCdma_LookupConfig(XPAR_XAXICDMA_0_BASEADDR);

    if (config_ptr == NULL) {
        xil_printf("CDMA configuration lookup failed\r\n");
		while (1);
	}

    status = XAxiCdma_CfgInitialize(inst_ptr, config_ptr, config_ptr->BaseAddress);

    if (status != XST_SUCCESS) {
        xil_printf("CDMA initialization failed\r\n");
		while (1);
	}

    XAxiCdma_Reset(inst_ptr);
    while (XAxiCdma_ResetIsDone(inst_ptr) == FALSE);

#if 0

    select_keyhole = XAxiCdma_SelectKeyHole(inst_ptr, XAXICDMA_KEYHOLE_WRITE, TRUE);

    if (select_keyhole != XST_SUCCESS) {
        xil_printf("CDMA keyhole select failed\r\n");
		while (1);
    }

#endif

    if (XAxiCdma_IsSimpleMode(inst_ptr) != TRUE) {
        xil_printf("CDMA not in simple mode\r\n");
        while(1);
    }

    return XST_SUCCESS;
}
