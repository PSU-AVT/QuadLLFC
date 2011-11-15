/*
 * Copyright (c) 2011 Gregory Haynes <greg@greghaynes.net>
 *
 * Licensed under the BSD license. See LICENSE for more information.
 */

#include "sysinit.h"
#include "esc.h"
#include "logging.h"
#include "core/uart.h"
#include "core/systick.h"
#include "sensors/itg3200.h"
#include "sensors/adxl345.h"
#include "afproto.h"

int main(void) {
	systemInit();
	systickInit(1);

	uartInit(115200);

	esc_init_all();
	esc_arm_all();

	// Init sensors
	itg3200Init();
	adxl345_Init();

	unsigned char *afproto_buff = afproto_get_buffer();
	GyroData gyro;
	AccelData accel;

	// Main loop
	while(1) {
		// Send gyro data
		itg3200GetData(&gyro);
		afproto_buff[0] = 4;
		*(float*)&afproto_buff[1] = gyro.X;
		*(float*)&afproto_buff[5] = gyro.Y;
		*(float*)&afproto_buff[9] = gyro.Z;
		int len = afproto_serialize_payload(afproto_buff, 13, uartGetOutputBuffer());
		uartSend(uartGetOutputBuffer(), len);

		// Send accelero data
		adxl345GetData(&accel);
		afproto_buff[0] = 5;
		*(float*)&afproto_buff[1] = accel.x;
		*(float*)&afproto_buff[5] = accel.y;
		*(float*)&afproto_buff[9] = accel.z;
		len = afproto_serialize_payload(afproto_buff, 13, uartGetOutputBuffer());
		uartSend(uartGetOutputBuffer(), len);

		systickDelay(10);
	}

	return 0;
}
