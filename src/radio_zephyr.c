/*
 * Copyright (c) 2020 Gerson Fernando Budke
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <device.h>
#include <drivers/sigfox.h>

#include <sigfox_types.h>
#include <sigfox_api.h>
#include <sigfox_zephyr.h>
#include <rf_api.h>

#include <logging/log.h>
LOG_MODULE_REGISTER(sigfox_radio, CONFIG_SIGFOX_LOG_LEVEL);

#define SIGFOX_RADIO_API_VERSION	"v2.7.0"

/**
 * SIGFOX RADIO ZEPHYR API
 */

sfx_u8 RF_API_init(sfx_rf_mode_t rf_mode)
{
	const struct device *dev = sigfox_get_ctx()->dev;
	const struct sigfox_modem_config *config = &sigfox_get_ctx()->config;
	const struct sigfox_driver_api *api = dev->api;

	LOG_DBG("");

	sigfox_get_ctx()->config.mode = rf_mode;

	return api->start(dev, config);
}

sfx_u8 RF_API_stop(void)
{
	const struct device *dev = sigfox_get_ctx()->dev;
	const struct sigfox_driver_api *api = dev->api;

	LOG_DBG("");

	return api->stop(dev);
}

sfx_u8 RF_API_send(sfx_u8 *stream,
		   sfx_modulation_type_t type,
		   sfx_u8 size)
{
	const struct device *dev = sigfox_get_ctx()->dev;
	const struct sigfox_driver_api *api = dev->api;

	LOG_DBG("");

	return api->send(dev, type, stream, size);
}

sfx_u8 RF_API_wait_frame(sfx_u8 *frame,
			 sfx_s16 *rssi,
			 sfx_rx_state_enum_t *state)
{
	const struct device *dev = sigfox_get_ctx()->dev;
	const struct sigfox_driver_api *api = dev->api;
	enum sigfox_rx_state rx_state;
	int ret;

	LOG_DBG("");

	ret = api->recv(dev, &rx_state, frame, rssi);
	if (ret) {
		return ret;
	}

	(*state) = rx_state;

	return SIGFOX_ERR_NONE;
}

sfx_u8 RF_API_wait_for_clear_channel(sfx_u8 cs_min,
				     sfx_s8 cs_threshold,
				     sfx_rx_state_enum_t *state)
{
	const struct device *dev = sigfox_get_ctx()->dev;
	const struct sigfox_driver_api *api = dev->api;
	enum sigfox_rx_state rx_state;
	int ret;

	LOG_DBG("");

	ret = api->cca(dev, &rx_state, cs_min, cs_threshold);
	if (ret) {
		return ret;
	}

	(*state) = rx_state;

	return SIGFOX_ERR_NONE;
}

sfx_u8 RF_API_get_version(sfx_u8 **version, sfx_u8 *size)
{
	(*version) = (sfx_u8 *) SIGFOX_RADIO_API_VERSION;
	(*size) = sizeof(SIGFOX_RADIO_API_VERSION);

	LOG_DBG("Version: %s, Size: %d", *version, *size);

	return SIGFOX_ERR_NONE;
}

#ifdef CONFIG_SIGFOX_ADDONS_FEATURE_API
sfx_u8 RF_API_start_continuous_transmission(sfx_modulation_type_t type)
{
	const struct device *dev = sigfox_get_ctx()->dev;
	const struct sigfox_driver_api *api = dev->api;

	LOG_DBG("");

	return api->tx_cont_start(dev, type);
}

sfx_u8 RF_API_stop_continuous_transmission(void)
{
	const struct device *dev = sigfox_get_ctx()->dev;
	const struct sigfox_driver_api *api = dev->api;

	LOG_DBG("");

	return api->tx_cont_stop(dev);
}

sfx_u8 RF_API_change_frequency(sfx_u32 frequency)
{
	const struct device *dev = sigfox_get_ctx()->dev;
	const struct sigfox_driver_api *api = dev->api;

	LOG_DBG("");

	return api->sw_freq(dev, frequency);
}
#endif /* CONFIG_SIGFOX_ADDONS_FEATURE_API */
