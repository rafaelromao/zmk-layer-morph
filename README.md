# ZMK Layer Morph Behavior

The Layer Morph behavior morphs a keypress based on whether a given layer is active.

## Usage

To load the module, add the following entries to remotes and projects in config/west.yml.

```dts
manifest:
  remotes:
    - name: zmkfirmware
      url-base: https://github.com/zmkfirmware
    - name: rafaelromao
      url-base: https://github.com/rafaelromao
  projects:
    - name: zmk
      remote: zmkfirmware
      revision: v0.3
      import: app/west.yml
    - name: zmk-layer-morph
      remote: rafaelromao
      revision: v0.3 # set to same as ZMK version above
  self:
    path: config
```

## Devicetree Binding

To use the layer morph behavior, you need to define it in your keymap file.

### Properties

- `bindings`: A phandle-array of two bindings. The first binding is the "normal" binding, and the second is the "morphed" binding.
- `layers`: The layer (or layers) to check for. If any of the specified layers are active, the morphed binding will be used. Can be a single layer like `<1>` or a list of layers like `<1 2>`.

### Example

Here is an example of how to define a layer morph behavior in your `.keymap` file:

```dts
/ {
    behaviors {
        // Define the layer-morph behavior
        my_layer_morph: layer_morph {
            compatible = "zmk,behavior-layer-morph";
            #binding-cells = <0>;
            // The layers to check for (e.g., layers 1 and 2)
            layers = <1 2>;
            // The bindings for the normal and morphed keys
            // Normal: 'A', Morphed: 'B'
            bindings = <&kp A>, <&kp B>;
        };
    };

    keymap {
        compatible = "zmk,keymap";

        default_layer {
            bindings = <&my_layer_morph>;
        };
    };
};
```

In this example, pressing the key will send `A`. If layer `1` or layer `2` is active, it will send `B` instead.

## Advanced Example: MacOS and Linux Layers

This example shows how to use the layer morph behavior to create a keymap that works for both MacOS and Linux.

The keymap has a default base layer for MacOS, and a secondary base layer for Linux that can be toggled on. A third layer contains shortcuts for copy and paste that will behave differently depending on whether the Linux layer is active.

```dts
/ {
    // Define layer names for clarity
    // 0: MacOS (default)
    // 1: Linux
    // 2: Shortcuts
    
    behaviors {
        // Layer morph for copy
        lm_copy: layer_morph_copy {
            compatible = "zmk,behavior-layer-morph";
            #binding-cells = <0>;
            layers = <1>; // Check for Linux layer
            bindings = <&kp LG(C)>, <&kp LC(C)>; // MacOS, Linux
        };

        // Layer morph for paste
        lm_paste: layer_morph_paste {
            compatible = "zmk,behavior-layer-morph";
            #binding-cells = <0>;
            layers = <1>; // Check for Linux layer
            bindings = <&kp LG(V)>, <&kp LC(V)>; // MacOS, Linux
        };
    };

    keymap {
        compatible = "zmk,keymap";

        macos_layer {
            bindings = <
                &kp A  &kp B  &tog 1    &mo 2
            >;
        };

        linux_layer {
            bindings = <
                &trans &trans &tog 1    &mo 2
            >;
        };

        shortcuts_layer {
            bindings = <
                &lm_copy &lm_paste &trans   &trans
            >;
        };
    };
};
```

### How it Works

1.  **Layers:**
    -   The `macos_layer` is the default layer (layer 0).
    -   The `linux_layer` is layer 1.
    -   The `shortcuts_layer` is layer 2.

2.  **Layer Switching:**
    -   On any of the base layers, pressing the third key (`&tog 1`) toggles the `linux_layer` on or off.
    -   Pressing and holding the fourth key (`&mo 2`) momentarily activates the `shortcuts_layer`.

3.  **Layer Morph:**
    -   When the `shortcuts_layer` is active, the first key triggers the `&lm_copy` behavior.
        -   If the `linux_layer` is **not** active (i.e., we are in MacOS mode), it sends `LG(C)` (Command-C).
        -   If the `linux_layer` is active, it sends `LC(C)` (Control-C).
    -   The second key triggers the `&lm_paste` behavior, which works similarly for pasting (`LG(V)` or `LC(V)`).

## Corne-ish Zen Example

You can see a variation of the example above implemented for my Corne-ish Zen [here](https://github.com/rafaelromao/zmk-config-zen-2/pull/1/files).

