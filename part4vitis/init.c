#include "init.h"

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

int init_CDMA(XAxiCdma_Config* cdma_config) {

    // Needs code

    return 0;
}
