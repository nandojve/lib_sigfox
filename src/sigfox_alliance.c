/*
 * Copyright (c) 2020 Gerson Fernando Budke
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <device.h>
#include <drivers/sigfox.h>

#include <sigfox_types.h>
#include <sigfox_api.h>
#include <sigfox_types.h>
#include <sigfox_zephyr.h>
#include <rf_api.h>


#include <logging/log.h>
LOG_MODULE_REGISTER(sigfox_network, CONFIG_SIGFOX_LOG_LEVEL);

static struct sigfox_context *_sfx_z_internal_ctx = NULL;

struct sigfox_context* sigfox_get_ctx(void)
{
	return _sfx_z_internal_ctx;
}

int sigfox_start(const struct device *dev,
		 struct sigfox_modem_config *config,
		 struct sigfox_rc *rc)
{
	const struct sigfox_driver_api *api =
		(const struct sigfox_driver_api *)dev->api;
	/* In FCC we can choose the macro channel to use by a 86 bits bitmask
	 * In this case we use the first 9 macro channels
	 */
	sfx_u32 config_words[3] = {1, 0, 0};

	if (_sfx_z_internal_ctx != NULL) {
		LOG_ERR("The context is already allocated.");
		return -EAGAIN;
	}

	_sfx_z_internal_ctx = api->ctx(dev);

	_sfx_z_internal_ctx->dev = dev;
	_sfx_z_internal_ctx->config = *config;
	_sfx_z_internal_ctx->rc = *rc;
	_sfx_z_internal_ctx->se = NULL;

	// TODO: Open SE

	if (SIGFOX_API_open((sfx_rc_t *) &_sfx_z_internal_ctx->rc)) {
		LOG_ERR("Sigfox Alliance could not be opened.");
		return -EIO;
	}

	/* Set the standard configuration with default channel to 1 */
	if (SIGFOX_API_set_std_config(config_words, 0)) {
		sigfox_stop(dev);
		LOG_ERR("Sigfox Alliance wrong argument.");
		return -EINVAL;
	}

	return 0;
}

int sigfox_stop(const struct device *dev)
{
	if (_sfx_z_internal_ctx == NULL) {
		LOG_ERR("No context allocated.");
		return -ESRCH;
	}

	SIGFOX_API_close();

	_sfx_z_internal_ctx = NULL;

	return 0;
}

int sigfox_send_frame(const struct device *dev,
		      uint8_t* tx_payload,
		      uint8_t* rx_payload,
		      uint8_t  tx_length)
{
	bool init_downlink = (rx_payload != NULL);

	ARG_UNUSED(dev);

	if (_sfx_z_internal_ctx == NULL) {
		LOG_ERR("No context allocated.");
		return -ESRCH;
	}

	if (tx_length > 12) {
		LOG_ERR("TX data length > 12 bytes.");
		return -EINVAL;
	}

	if (SIGFOX_API_send_frame(tx_payload, tx_length,
				  rx_payload, 2, init_downlink) != 0) {
		LOG_ERR("TX data length > 12 bytes.");
		return -EIO;
	}

	return 0;
}