
if(CONFIG_OWNTECH_DATA_ACQUISITION)
  zephyr_include_directories(./public_api)

  zephyr_library()
  zephyr_library_sources(
    ./adc/adc.c
    ./adc/adc_channels.c
    ./adc/adc_core.c
    ./adc/adc_helper.c
    ./dma/dma.c
    ./data_dispatch/data_dispatch.c
	./data_conversion/data_conversion.c
    ./public_api/DataAcquisition.cpp
   )
endif()
