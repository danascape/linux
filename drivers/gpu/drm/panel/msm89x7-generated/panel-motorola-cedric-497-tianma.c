// SPDX-License-Identifier: GPL-2.0-only
// Copyright (c) 2024 FIXME
// Generated with linux-mdss-dsi-panel-driver-generator from vendor device tree:
//   Copyright (c) 2013, The Linux Foundation. All rights reserved. (FIXME)

#include <linux/delay.h>
#include <linux/gpio/consumer.h>
#include <linux/module.h>
#include <linux/of.h>

#include <video/mipi_display.h>

#include <drm/drm_mipi_dsi.h>
#include <drm/drm_modes.h>
#include <drm/drm_panel.h>

struct tianma_497_v0 {
	struct drm_panel panel;
	struct mipi_dsi_device *dsi;
	struct gpio_desc *reset_gpio;
	bool prepared;
};

static inline struct tianma_497_v0 *to_tianma_497_v0(struct drm_panel *panel)
{
	return container_of(panel, struct tianma_497_v0, panel);
}

static void tianma_497_v0_reset(struct tianma_497_v0 *ctx)
{
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	usleep_range(2000, 3000);
	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	usleep_range(2000, 3000);
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	usleep_range(2000, 3000);
	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	usleep_range(2000, 3000);
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	msleep(20);
}

static int tianma_497_v0_on(struct tianma_497_v0 *ctx)
{
	struct mipi_dsi_device *dsi = ctx->dsi;
	struct device *dev = &dsi->dev;
	int ret;

	mipi_dsi_dcs_write_seq(dsi, 0xff, 0xee);
	mipi_dsi_dcs_write_seq(dsi, 0x18, 0x40);
	usleep_range(10000, 11000);
	mipi_dsi_dcs_write_seq(dsi, 0x18, 0x00);
	usleep_range(10000, 11000);
	mipi_dsi_dcs_write_seq(dsi, 0xff, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0xfb, 0x01);
	mipi_dsi_dcs_write_seq(dsi, 0xd3, 0x0a);
	mipi_dsi_dcs_write_seq(dsi, 0xd4, 0x0e);

	ret = mipi_dsi_dcs_exit_sleep_mode(dsi);
	if (ret < 0) {
		dev_err(dev, "Failed to exit sleep mode: %d\n", ret);
		return ret;
	}
	msleep(120);

	ret = mipi_dsi_dcs_set_display_brightness(dsi, 0x00cc);
	if (ret < 0) {
		dev_err(dev, "Failed to set display brightness: %d\n", ret);
		return ret;
	}

	mipi_dsi_dcs_write_seq(dsi, MIPI_DCS_WRITE_CONTROL_DISPLAY, 0x2c);
	mipi_dsi_dcs_write_seq(dsi, MIPI_DCS_WRITE_POWER_SAVE, 0x01);

	ret = mipi_dsi_dcs_set_display_on(dsi);
	if (ret < 0) {
		dev_err(dev, "Failed to set display on: %d\n", ret);
		return ret;
	}

	return 0;
}

static int tianma_497_v0_off(struct tianma_497_v0 *ctx)
{
	struct mipi_dsi_device *dsi = ctx->dsi;
	struct device *dev = &dsi->dev;
	int ret;

	ret = mipi_dsi_dcs_set_display_off(dsi);
	if (ret < 0) {
		dev_err(dev, "Failed to set display off: %d\n", ret);
		return ret;
	}

	ret = mipi_dsi_dcs_enter_sleep_mode(dsi);
	if (ret < 0) {
		dev_err(dev, "Failed to enter sleep mode: %d\n", ret);
		return ret;
	}
	msleep(120);

	return 0;
}

static int tianma_497_v0_prepare(struct drm_panel *panel)
{
	struct tianma_497_v0 *ctx = to_tianma_497_v0(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	if (ctx->prepared)
		return 0;

	tianma_497_v0_reset(ctx);

	ret = tianma_497_v0_on(ctx);
	if (ret < 0) {
		dev_err(dev, "Failed to initialize panel: %d\n", ret);
		gpiod_set_value_cansleep(ctx->reset_gpio, 1);
		return ret;
	}

	ctx->prepared = true;
	return 0;
}

static int tianma_497_v0_unprepare(struct drm_panel *panel)
{
	struct tianma_497_v0 *ctx = to_tianma_497_v0(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	if (!ctx->prepared)
		return 0;

	ret = tianma_497_v0_off(ctx);
	if (ret < 0)
		dev_err(dev, "Failed to un-initialize panel: %d\n", ret);

	gpiod_set_value_cansleep(ctx->reset_gpio, 1);

	ctx->prepared = false;
	return 0;
}

static const struct drm_display_mode tianma_497_v0_mode = {
	.clock = (1080 + 120 + 20 + 120) * (1920 + 14 + 2 + 8) * 60 / 1000,
	.hdisplay = 1080,
	.hsync_start = 1080 + 120,
	.hsync_end = 1080 + 120 + 20,
	.htotal = 1080 + 120 + 20 + 120,
	.vdisplay = 1920,
	.vsync_start = 1920 + 14,
	.vsync_end = 1920 + 14 + 2,
	.vtotal = 1920 + 14 + 2 + 8,
	.width_mm = 62,
	.height_mm = 110,
};

static int tianma_497_v0_get_modes(struct drm_panel *panel,
				   struct drm_connector *connector)
{
	struct drm_display_mode *mode;

	mode = drm_mode_duplicate(connector->dev, &tianma_497_v0_mode);
	if (!mode)
		return -ENOMEM;

	drm_mode_set_name(mode);

	mode->type = DRM_MODE_TYPE_DRIVER | DRM_MODE_TYPE_PREFERRED;
	connector->display_info.width_mm = mode->width_mm;
	connector->display_info.height_mm = mode->height_mm;
	drm_mode_probed_add(connector, mode);

	return 1;
}

static const struct drm_panel_funcs tianma_497_v0_panel_funcs = {
	.prepare = tianma_497_v0_prepare,
	.unprepare = tianma_497_v0_unprepare,
	.get_modes = tianma_497_v0_get_modes,
};

static int tianma_497_v0_probe(struct mipi_dsi_device *dsi)
{
	struct device *dev = &dsi->dev;
	struct tianma_497_v0 *ctx;
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
			  MIPI_DSI_CLOCK_NON_CONTINUOUS | MIPI_DSI_MODE_LPM;

	drm_panel_init(&ctx->panel, dev, &tianma_497_v0_panel_funcs,
		       DRM_MODE_CONNECTOR_DSI);
	ctx->panel.prepare_prev_first = true;

	ret = drm_panel_of_backlight(&ctx->panel);
	if (ret)
		return dev_err_probe(dev, ret, "Failed to get backlight\n");

	drm_panel_add(&ctx->panel);

	ret = mipi_dsi_attach(dsi);
	if (ret < 0) {
		dev_err(dev, "Failed to attach to DSI host: %d\n", ret);
		drm_panel_remove(&ctx->panel);
		return ret;
	}

	return 0;
}

static void tianma_497_v0_remove(struct mipi_dsi_device *dsi)
{
	struct tianma_497_v0 *ctx = mipi_dsi_get_drvdata(dsi);
	int ret;

	ret = mipi_dsi_detach(dsi);
	if (ret < 0)
		dev_err(&dsi->dev, "Failed to detach from DSI host: %d\n", ret);

	drm_panel_remove(&ctx->panel);
}

static const struct of_device_id tianma_497_v0_of_match[] = {
	{ .compatible = "motorola,cedric-497-tianma" }, // FIXME
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, tianma_497_v0_of_match);

static struct mipi_dsi_driver tianma_497_v0_driver = {
	.probe = tianma_497_v0_probe,
	.remove = tianma_497_v0_remove,
	.driver = {
		.name = "panel-motorola-cedric-497-tianma",
		.of_match_table = tianma_497_v0_of_match,
	},
};
module_mipi_dsi_driver(tianma_497_v0_driver);

MODULE_AUTHOR("linux-mdss-dsi-panel-driver-generator <fix@me>"); // FIXME
MODULE_DESCRIPTION("DRM driver for mipi_mot_vid_tianma_1080p_497");
MODULE_LICENSE("GPL");
