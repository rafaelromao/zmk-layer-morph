/*
 * Copyright (c) 2025 Rafael Romão
 *
 * SPDX-License-Identifier: MIT
 */

#define DT_DRV_COMPAT zmk_behavior_layer_morph

#include <drivers/behavior.h>
#include <zephyr/device.h>
#include <zephyr/logging/log.h>
#include <zmk/behavior.h>
#include <zmk/behavior_queue.h>
#include <zmk/keymap.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#if DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT)

struct behavior_layer_morph_config {
  struct zmk_behavior_binding normal_binding;
  struct zmk_behavior_binding morphed_binding;
  const uint32_t *layers;
  uint8_t layers_len;
};

static bool is_any_layer_active(const struct behavior_layer_morph_config *cfg) {
  for (int i = 0; i < cfg->layers_len; i++) {
    if (zmk_keymap_layer_active((uint8_t)cfg->layers[i])) {
      return true;
    }
  }
  return false;
}

static int
on_layer_morph_binding_pressed(struct zmk_behavior_binding *binding,
                               struct zmk_behavior_binding_event event) {
  const struct device *dev = zmk_behavior_get_binding(binding->behavior_dev);
  const struct behavior_layer_morph_config *cfg = dev->config;

  if (is_any_layer_active(cfg)) {
    return zmk_behavior_queue_add(&event, cfg->morphed_binding, true, 0);
  } else {
    return zmk_behavior_queue_add(&event, cfg->normal_binding, true, 0);
  }
}

static int
on_layer_morph_binding_released(struct zmk_behavior_binding *binding,
                                struct zmk_behavior_binding_event event) {
  const struct device *dev = zmk_behavior_get_binding(binding->behavior_dev);
  const struct behavior_layer_morph_config *cfg = dev->config;

  if (is_any_layer_active(cfg)) {
    return zmk_behavior_queue_add(&event, cfg->morphed_binding, false, 0);
  } else {
    return zmk_behavior_queue_add(&event, cfg->normal_binding, false, 0);
  }
}

static const struct behavior_driver_api behavior_layer_morph_driver_api = {
    .binding_pressed = on_layer_morph_binding_pressed,
    .binding_released = on_layer_morph_binding_released,
};

static int behavior_layer_morph_init(const struct device *dev) { return 0; };

#define LM_INST(i)                                                                 \
  static const uint32_t behavior_layer_morph_layers_##i[] = DT_INST_PROP(i, layers); \
  static struct behavior_layer_morph_config behavior_layer_morph_config_##i = {      \
      .normal_binding = ZMK_KEYMAP_EXTRACT_BINDING(0, DT_DRV_INST(i)),                \
      .morphed_binding = ZMK_KEYMAP_EXTRACT_BINDING(1, DT_DRV_INST(i)),               \
      .layers = behavior_layer_morph_layers_##i,                                     \
      .layers_len = DT_INST_PROP_LEN(i, layers),                                     \
  };                                                                                 \
  BEHAVIOR_DT_INST_DEFINE(i, behavior_layer_morph_init, NULL, NULL,                   \
                          &behavior_layer_morph_config_##i, POST_KERNEL,             \
                          CONFIG_KERNEL_INIT_PRIORITY_DEFAULT,                        \
                          &behavior_layer_morph_driver_api);

DT_INST_FOREACH_STATUS_OKAY(LM_INST)

#endif /* DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT) */
