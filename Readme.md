# Mechacon Crash test application

This program will check your console mechacon version and boot rom version to identify if your console is vulnerable to the mechacon crash

Mechacon crash is a "feature" introduced by sony that makes retail PS2 DSP chips have a 50% chance of killing the lens coils when reading backup discs with bogus error correction sector data.

The affected DSP revision is `CXD3098Q`

The following models are related
Model numbers | Status | Notes
----- | ------ | -----
`SCPH-39xxx`  | **UNSAFE** | On this model, mechacon crash seems to have less chances of triggering
`SCPH-50xxx`  | **UNSAFE** | This model has a considerable chance of suffering mechacon crash
`SCPH-70xxx`  | **UNSAFE** | This model has a lot of chances of suffering mechacon crash, however, some of the late batches of this model have a factory protection added by sony after getting sued on the USA. these models are Identified by mechacon version being 6.04 or newer
`SCPH-75xxx`   | **SAFE** | these models have a factory protection
`SCPH-77xxx`  | **SAFE** | these models have a factory protection
`SCPH-79xxx`  | **SAFE** | these models have a newer revision of the DSP chip wich no longer crashes
`SCPH-90xxx`  | **SAFE** | these models have a newer revision of the DSP chip wich no longer crashes
`DESR-xxxx`   | **SAFE** | despite being based on 50k series, PSX DESR cannot suffer mechacon crash
