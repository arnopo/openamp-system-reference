# Zephyr Example Application

This repository contains legacy applications and tests examples.


## build legacy application


### Initialization

The first step is to initialize the workspace folder (``my-workspace``) where
the examples and all Zephyr modules will be cloned. You can do
that by running:

```shell
# initialize my-workspace for the example-application (main branch)
west init -m https://github.com/OpenAMP/openamp-system-reference --mf examples/legacy_apps/west.yml --mr main my-workspace
# update modules
cd my-workspace
west update
```

# Build legacy_apps

Legacy apps are moved from open_amp library repository.

```
export PROJECT_ROOT=$PWD
```

## Build libmetal
```
  $ cd $PROJECT_ROOT/libmetal
  $ cmake . -Bbuild -DCMAKE_INSTALL_PREFIX= $PROJECT_ROOT/target
  $ make -C build VERBOSE=1
```

## Build open_amp
```
  $ cd $PROJECT_ROOT/open-amp
  $ cmake . -Bbuild -DCMAKE_INCLUDE_PATH=$PROJECT_ROOT/libmetal/build/lib/include/   -DCMAKE_LIBRARY_PATH=$PROJECT_ROOT/libmetal/build/lib/ -DCMAKE_INSTALL_PREFIX=$PROJECT_ROOT/target
  $ make -C build VERBOSE=1 install
```
## Build legacy Apps
```
  $ cd $PROJECT_ROOT/openamp-system-reference/examples/legacy_apps
  $ cmake -Bbuild \
-DCMAKE_INCLUDE_PATH="$PROJECT_ROOT/libmetal/build/lib/include/;$PROJECT_ROOT/open-amp/build/lib/include/" \
-DCMAKE_LIBRARY_PATH="$PROJECT_ROOT/libmetal/build/lib/;$PROJECT_ROOT/open-amp/build/lib/" \
-DCMAKE_INSTALL_PREFIX=$PROJECT_ROOT/target
$ make -C build VERBOSE=1 install
```

## Run application on a Linux PC
It is possible to run application on a Linux PC to communicate between two Linux processes.

```
  $ cd $PROJECT_ROOT/target
  $ echo "################### run ping test #####################"
  $ LD_LIBRARY_PATH=./lib  ./bin/rpmsg-echo-static &
  $ sleep 1
  $ LD_LIBRARY_PATH=./lib  ./bin//msg-test-rpmsg-ping-static 1
  $ 
  $ echo "################### run ping test #####################"
  $ LD_LIBRARY_PATH=./lib  ./bin/rpmsg-nocopy-echo-static &
  $ sleep 1
  $ LD_LIBRARY_PATH=./lib  ./bin//rpmsg-nocopy-ping-static  1
```
