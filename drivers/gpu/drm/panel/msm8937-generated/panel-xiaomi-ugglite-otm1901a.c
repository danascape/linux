// SPDX-License-Identifier: GPL-2.0-only
// Copyright (c) 2024 FIXME
// Generated with linux-mdss-dsi-panel-driver-generator from vendor device tree:
//   Copyright (c) 2013, The Linux Foundation. All rights reserved. (FIXME)

#include <linux/backlight.h>
#include <linux/delay.h>
#include <linux/gpio/consumer.h>
#include <linux/module.h>
#include <linux/of.h>

#include <video/mipi_display.h>

#include <drm/drm_mipi_dsi.h>
#include <drm/drm_modes.h>
#include <drm/drm_panel.h>
#include <drm/drm_probe_helper.h>

struct tm_otm1901a {
	struct drm_panel panel;
	struct mipi_dsi_device *dsi;
	struct gpio_desc *reset_gpio;
};

static inline struct tm_otm1901a *to_tm_otm1901a(struct drm_panel *panel)
{
	return container_of(panel, struct tm_otm1901a, panel);
}

static void tm_otm1901a_reset(struct tm_otm1901a *ctx)
{
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	usleep_range(10000, 11000);
	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	usleep_range(1000, 2000);
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	msleep(20);
}

static int tm_otm1901a_on(struct tm_otm1901a *ctx)
{
	struct mipi_dsi_device *dsi = ctx->dsi;
	struct device *dev = &dsi->dev;
	int ret;

	mipi_dsi_dcs_write_seq(dsi, 0x00, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0xff, 0x19, 0x01, 0x01);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0x80);
	mipi_dsi_dcs_write_seq(dsi, 0xff, 0x19, 0x01);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0x1c, 0x33);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0x1c, 0x05);

	ret = mipi_dsi_dcs_set_column_address(dsi, 0x0000, 0x02cf);
	if (ret < 0) {
		dev_err(dev, "Failed to set column address: %d\n", ret);
		return ret;
	}

	ret = mipi_dsi_dcs_set_page_address(dsi, 0x0000, 0x04ff);
	if (ret < 0) {
		dev_err(dev, "Failed to set page address: %d\n", ret);
		return ret;
	}

	mipi_dsi_dcs_write_seq(dsi, 0x00, 0x90);
	mipi_dsi_dcs_write_seq(dsi, 0xd7, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0x91);
	mipi_dsi_dcs_write_seq(dsi, 0xd7, 0xc8);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0xba);
	mipi_dsi_dcs_write_seq(dsi, 0xc0, 0xc2, 0x01);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0xb0);
	mipi_dsi_dcs_write_seq(dsi, 0xca, 0x02);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0x80);
	mipi_dsi_dcs_write_seq(dsi, 0xca, 0x80);

	ret = mipi_dsi_dcs_set_tear_on(dsi, MIPI_DSI_DCS_TEAR_MODE_VBLANK);
	if (ret < 0) {
		dev_err(dev, "Failed to set tear on: %d\n", ret);
		return ret;
	}

	mipi_dsi_dcs_write_seq(dsi, 0x00, 0xc1);
	mipi_dsi_dcs_write_seq(dsi, 0xc5, 0x77);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0x86);
	mipi_dsi_dcs_write_seq(dsi, 0xf3, 0xe0);
	mipi_dsi_dcs_write_seq(dsi, 0x11, 0x00);
	msleep(30);
	mipi_dsi_dcs_write_seq(dsi, 0x29, 0x00);
	usleep_range(10000, 11000);
	mipi_dsi_dcs_write_seq(dsi, MIPI_DCS_WRITE_CONTROL_DISPLAY, 0x24);
	mipi_dsi_dcs_write_seq(dsi, MIPI_DCS_WRITE_POWER_SAVE, 0x00);

	return 0;
}

static int tm_otm1901a_off(struct tm_otm1901a *ctx)
{
	struct mipi_dsi_device *dsi = ctx->dsi;
	struct device *dev = &dsi->dev;
	int ret;

	ret = mipi_dsi_dcs_set_display_off(dsi);
	if (ret < 0) {
		dev_err(dev, "Failed to set display off: %d\n", ret);
		return ret;
	}
	msleep(53);

	ret = mipi_dsi_dcs_enter_sleep_mode(dsi);
	if (ret < 0) {
		dev_err(dev, "Failed to enter sleep mode: %d\n", ret);
		return ret;
	}
	msleep(120);

	return 0;
}

static int tm_otm1901a_prepare(struct drm_panel *panel)
{
	struct tm_otm1901a *ctx = to_tm_otm1901a(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	tm_otm1901a_reset(ctx);

	ret = tm_otm1901a_on(ctx);
	if (ret < 0) {
		dev_err(dev, "Failed to initialize panel: %d\n", ret);
		gpiod_set_value_cansleep(ctx->reset_gpio, 1);
		return ret;
	}

	return 0;
}

static int tm_otm1901a_unprepare(struct drm_panel *panel)
{
	struct tm_otm1901a *ctx = to_tm_otm1901a(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	ret = tm_otm1901a_off(ctx);
	if (ret < 0)
		dev_err(dev, "Failed to un-initialize panel: %d\n", ret);

	gpiod_set_value_cansleep(ctx->reset_gpio, 1);

	return 0;
}

static const struct drm_display_mode tm_otm1901a_mode = {
	.clock = (720 + 96 + 4 + 96) * (1280 + 14 + 1 + 9) * 60 / 1000,
	.hdisplay = 720,
	.hsync_start = 720 + 96,
	.hsync_end = 720 + 96 + 4,
	.htotal = 720 + 96 + 4 + 96,
	.vdisplay = 1280,
	.vsync_start = 1280 + 14,
	.vsync_end = 1280 + 14 + 1,
	.vtotal = 1280 + 14 + 1 + 9,
	.width_mm = 68,
	.height_mm = 121,
	.type = DRM_MODE_TYPE_DRIVER,
};

static int tm_otm1901a_get_modes(struct drm_panel *panel,
				 struct drm_connector *connector)
{
	return drm_connector_helper_get_modes_fixed(connector, &tm_otm1901a_mode);
}

static const struct drm_panel_funcs tm_otm1901a_panel_funcs = {
	.prepare = tm_otm1901a_prepare,
	.unprepare = tm_otm1901a_unprepare,
	.get_modes = tm_otm1901a_get_modes,
};

static int tm_otm1901a_bl_update_status(struct backlight_device *bl)
{
	struct mipi_dsi_device *dsi = bl_get_data(bl);
	u16 brightness = backlight_get_brightness(bl);
	int ret;

	dsi->mode_flags &= ~MIPI_DSI_MODE_LPM;

	ret = mipi_dsi_dcs_set_display_brightness(dsi, brightness);
	if (ret < 0)
		return ret;

	dsi->mode_flags |= MIPI_DSI_MODE_LPM;

	return 0;
}

static const struct backlight_ops tm_otm1901a_bl_ops = {
	.update_status = tm_otm1901a_bl_update_status,
};

static struct backlight_device *
tm_otm1901a_create_backlight(struct mipi_dsi_device *dsi)
{
	struct device *dev = &dsi->dev;
	const struct backlight_properties props = {
		.type = BACKLIGHT_RAW,
		.brightness = 255,
		.max_brightness = 255,
	};

	return devm_backlight_device_register(dev, dev_name(dev), dev, dsi,
					      &tm_otm1901a_bl_ops, &props);
}

static int tm_otm1901a_probe(struct mipi_dsi_device *dsi)
{
	struct device *dev = &dsi->dev;
	struct tm_otm1901a *ctx;
	int ret;

	ctx = devm_kzalloc(dev, sizeof(*ctx), GFP_KERNEL);
	if (!ctx)
		return -ENOMEM;

	ctx->reset_gpio = devm_gpiod_get(dev, "reset", GPIOD_OUT_HIGH);
	if (IS_ERR(ctx->reset_gpio))
		return dev_err_probe(dev, PTR_ERR(ctx->reset_gpio),
				     "Failed to get reset-gpios\n");

	ctx->dsi = dsi;
	mipi_dsi_set_drvdata(dsi, ctx);

	dsi->lanes = 4;
	dsi->format = MIPI_DSI_FMT_RGB888;
	dsi->mode_flags = MIPI_DSI_MODE_VIDEO | MIPI_DSI_MODE_VIDEO_BURST |
			  MIPI_DSI_MODE_VIDEO_HSE |
			  MIPI_DSI_CLOCK_NON_CONTINUOUS | MIPI_DSI_MODE_LPM;

	drm_panel_init(&ctx->panel, dev, &tm_otm1901a_panel_funcs,
		       DRM_MODE_CONNECTOR_DSI);
	// ctx->panel.prepare_prev_first = true;

	ctx->panel.backlight = tm_otm1901a_create_backlight(dsi);
	if (IS_ERR(ctx->panel.backlight))
		return dev_err_probe(dev, PTR_ERR(ctx->panel.backlight),
				     "Failed to create backlight\n");

	drm_panel_add(&ctx->panel);

	ret = mipi_dsi_attach(dsi);
	if (ret < 0) {
		drm_panel_remove(&ctx->panel);
		return dev_err_probe(dev, ret, "Failed to attach to DSI host\n");
	}

	return 0;
}

static void tm_otm1901a_remove(struct mipi_dsi_device *dsi)
{
	struct tm_otm1901a *ctx = mipi_dsi_get_drvdata(dsi);
	int ret;

	ret = mipi_dsi_detach(dsi);
	if (ret < 0)
		dev_err(&dsi->dev, "Failed to detach from DSI host: %d\n", ret);

	drm_panel_remove(&ctx->panel);
}

static const struct of_device_id tm_otm1901a_of_match[] = {
	{ .compatible = "xiaomi,ugglite-otm1901a" }, // FIXME
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, tm_otm1901a_of_match);

static struct mipi_dsi_driver tm_otm1901a_driver = {
	.probe = tm_otm1901a_probe,
	.remove = tm_otm1901a_remove,
	.driver = {
		.name = "panel-tm-otm1901a",
		.of_match_table = tm_otm1901a_of_match,
	},
};
module_mipi_dsi_driver(tm_otm1901a_driver);

MODULE_AUTHOR("linux-mdss-dsi-panel-driver-generator <fix@me>"); // FIXME
MODULE_DESCRIPTION("DRM driver for tm otm1901a 720p video mode dsi panel");
MODULE_LICENSE("GPL");
