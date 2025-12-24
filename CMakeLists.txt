if ((NOT CONFIG_ZMK_SPLIT) OR CONFIG_ZMK_SPLIT_ROLE_CENTRAL)
 target_sources_ifdef(CONFIG_ZMK_BEHAVIOR_LAYER_MORPH app PRIVATE ../src/behavior_layer_morph.c)
endif()