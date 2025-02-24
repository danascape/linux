// SPDX-License-Identifier: GPL-2.0-only
/*
 * Initialization protocol for ISHTP driver
 *
 * Copyright (c) 2003-2016, Intel Corporation.
 */

#include <linux/devm-helpers.h>
#include <linux/export.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include "ishtp-dev.h"
#include "hbm.h"
#include "client.h"
#include "loader.h"

/**
 * ishtp_device_init() - ishtp device init
 * @dev: ISHTP device instance
 *
 * After ISHTP device is alloacted, this function is used to initialize
 * each field which includes spin lock, work struct and lists
 */
void ishtp_device_init(struct ishtp_device *dev)
{
	int ret;

	dev->dev_state = ISHTP_DEV_INITIALIZING;
	INIT_LIST_HEAD(&dev->cl_list);
	INIT_LIST_HEAD(&dev->device_list);
	dev->rd_msg_fifo_head = 0;
	dev->rd_msg_fifo_tail = 0;
	spin_lock_init(&dev->rd_msg_spinlock);

	init_waitqueue_head(&dev->wait_hbm_recvd_msg);
	init_waitqueue_head(&dev->wait_loader_recvd_msg);
	spin_lock_init(&dev->read_list_spinlock);
	spin_lock_init(&dev->device_lock);
	spin_lock_init(&dev->device_list_lock);
	spin_lock_init(&dev->cl_list_lock);
	spin_lock_init(&dev->fw_clients_lock);
	INIT_WORK(&dev->bh_hbm_work, bh_hbm_work_fn);

	bitmap_zero(dev->host_clients_map, ISHTP_CLIENTS_MAX);
	dev->open_handle_count = 0;

	/*
	 * Reserving client ID 0 for ISHTP Bus Message communications
	 */
	bitmap_set(dev->host_clients_map, 0, 1);

	INIT_LIST_HEAD(&dev->read_list.list);

	ret = devm_work_autocancel(dev->devc, &dev->work_fw_loader, ishtp_loader_work);
	if (ret)
		dev_err_probe(dev->devc, ret, "Failed to initialise FW loader work\n");
}
EXPORT_SYMBOL(ishtp_device_init);

/**
 * ishtp_start() - Start ISH processing
 * @dev: ISHTP device instance
 *
 * Start ISHTP processing by sending query subscriber message
 *
 * Return: 0 on success else -ENODEV
 */
int ishtp_start(struct ishtp_device *dev)
{
	if (ishtp_hbm_start_wait(dev)) {
		dev_err(dev->devc, "HBM haven't started");
		goto err;
	}

	/* suspend & resume notification - send QUERY_SUBSCRIBERS msg */
	ishtp_query_subscribers(dev);

	return 0;
err:
	dev_err(dev->devc, "link layer initialization failed.\n");
	dev->dev_state = ISHTP_DEV_DISABLED;
	return -ENODEV;
}
EXPORT_SYMBOL(ishtp_start);
