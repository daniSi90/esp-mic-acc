set(srcs_common  main.c
                 peripheral.c)

set(includes "")

set(priv_requires esp_microphone
                  esp-analog-microphone
                  esp_driver_i2s
                  udp_handler
                  spi_flash)

idf_component_register(SRCS ${srcs_common}
                       INCLUDE_DIRS ${includes}
                       PRIV_REQUIRES ${priv_requires})
