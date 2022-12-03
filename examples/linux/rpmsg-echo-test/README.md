# app: echo_test

## Description:

  This demo uses kernel rpmsg framework to send various size of data buffer to remote
  processor and validates integrity of received buffer from remote processor.
  If buffer data does not match, then number of different bytes are reported on
  console

## Remote processor firmware: Xilinx ZynqMP cortex-r5 platform

  https://github.com/OpenAMP/open-amp/blob/main/apps/examples/echo/rpmsg-echo.c