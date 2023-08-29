// SPDX-License-Identifier: Apache-2.0

/* Copyright (c) 2024, STMicroelectronics */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/i2c/target/eeprom.h>
#include <zephyr/drivers/ipm.h>
#include <zephyr/ipc/virtio_transport.h>
#include <zephyr/kernel.h>

#include <openamp/open_amp.h>
#include <metal/device.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(virtio_i2c_ram, LOG_LEVEL_DBG);

#define I2C_REMOTE_DISP_ADDR 0x3D

/* constant derivated from linker symbols */

#define STACKSIZE (4096)
#define PRIORITY	K_PRIO_PREEMPT(2)


#define RAM_0_ADDR	0x54
#define RAM_1_ADDR	0x56

#define I2C_RAM_SIZE 20
#define I2C_NUM_RAM  2

static K_SEM_DEFINE(data_sem, 0, 1);


struct i2c_ram_t{
	uint8_t i2c_addr;
	uint8_t padding;
	uint8_t offset;
	uint8_t buff[I2C_RAM_SIZE];
};


static struct i2c_ram_t i2c_rams[I2C_NUM_RAM] = {
	{
		.i2c_addr = RAM_0_ADDR,
		.buff = "123456789abcdefghij",
	},
	{
		.i2c_addr = RAM_1_ADDR,
		.buff = "klmnopqrstuvwxyz!:;",
	}
};

struct virtio_i2c_ram_context {
	uint32_t buffer_idx;
	struct virtio_msg_bus_device vmsg_bus;
	const struct virtio_msg_bus_cfg vmsg_bus_cfg;
	struct i2c_target_config config;
	struct i2c_ram_t *ram;
};

static struct virtio_i2c_ram_context ctx[I2C_NUM_RAM];

static int ram_i2c_read(struct i2c_target_config *config, uint8_t **ptr, uint32_t *len)
{
	struct virtio_i2c_ram_context *ctx = CONTAINER_OF(config,
						struct virtio_i2c_ram_context,
						config);
	struct i2c_ram_t *ram =ctx->ram;

	if(I2C_RAM_SIZE < ram->offset)
		return -EINVAL;
	*ptr = &ram->buff[ram->offset];
	*len = I2C_RAM_SIZE - ram->offset ;

	return 0;
}

static void ram_i2c_write(struct i2c_target_config *config, uint8_t *ptr, uint32_t len)
{
	struct virtio_i2c_ram_context *ctx = CONTAINER_OF(config,
						struct virtio_i2c_ram_context,
						config);
	struct i2c_ram_t *ram =ctx->ram;

	/* The first byte is offset */
	ram->offset = *ptr;
	memcpy(&ram->buff[ram->offset], ptr + 1, len - 1);
}

static const struct i2c_target_callbacks i2c_virtio_callbacks = {
	.buf_write_received = ram_i2c_write,
	.buf_read_requested = ram_i2c_read,
};

void virtio_msg_bus(void *u1, void *u2, void *u3)
{
	const struct device *ipc_instance;
	const struct device *virtio_i2c_dev;
	int ret;

	printk("\r\nOpenAMP[remote] virtio msg bus demo started\r\n");

	ipc_instance = DEVICE_DT_GET(DT_NODELABEL(ipc0));
	virtio_i2c_dev = DEVICE_DT_GET(DT_NODELABEL(i2c_virtio_dev0));

	for (int i = 0; i < I2C_NUM_RAM; i++) {
		ctx[i].config.address = i2c_rams[i].i2c_addr;
		ctx[i].config.callbacks = &i2c_virtio_callbacks;
		ctx[i].ram = &i2c_rams[i];
		ret = i2c_target_register(virtio_i2c_dev, &ctx[i].config);
		if (ret) {
			LOG_ERR("Failed to register I2C SLAVE %#x\n", ctx[i].config.address);
			goto task_end;
		}
	}


	ret = ipc_virtio_transport_open_instance(ipc_instance);
	if (ret < 0 && ret != -EALREADY) {
		printk("ipc_service_open_instance() failure\n");
		return;
	}

	while(1);

task_end:
	LOG_DBG("sensor task ended\n");
}

K_THREAD_DEFINE(virtio_trans_thread_id, STACKSIZE, virtio_msg_bus, NULL, NULL, NULL, PRIORITY, 0, 0);
