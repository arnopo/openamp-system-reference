# Zephyr Example Application

This repository contains legacy applications and tests examples.


## build legacy application


### Initialization

The first step is to initialize the workspace folder (``my-workspace``) where
the examples and all Zephyr modules will be cloned. You can do
that by running:

```shell
# initialize my-workspace for the example-application (main branch)
west init -m https://github.com/OpenAMP/openamp-system-reference/examples/legacy_apps/ --mr main my-workspace
# update modules
cd my-workspace
west update
```

### Build and run on a board

The application can be built by running:

```shell
west build -b $BOARD `$ZEPHYR_EXAMPLE`
```

where `$BOARD` is the target board and `$ZEPHYR_EXAMPLE`.
Note that Zephyr sample boards may be used if an appropriate overlay is provided (see `app/boards`).

A sample debug configuration is also provided. You can apply it by running:

```shell
west build -b $BOARD $ZEPHYR_EXAMPLE -- -DOVERLAY_CONFIG=debug.conf
```

Example to compile rpmsg_multi_services on stm32mp157 discovery board:

```shell
west build -b stm32mp157c_dk2 openamp-system-reference/examples/zephyr/rpmsg_multi_services
```

### Running on a board

We consider here board on which the Zephyr image is running on a coprocessor.

1) Once you have built the application copy it on target filesystem.
2) Load the Zephyr firmware and start the coprocessor depending on the board.

Example of a Zephyr firmware image loading by the Linux kernel remoteproc framework.

```shell
cp $ZEPHYR_EXAMPLE.elf /lib/modules/
echo $ZEPHYR_EXAMPLE.elf > /sys/class/remoteproc/remoteproc0/firmware
echo start >/sys/class/remoteproc/remoteproc0/state
```

### Running in an Emulator

To be described