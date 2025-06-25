set(srcs_common  "esp_microphone.c")

set(includes "include")

set(priv_requires esp_driver_i2s)

idf_component_register(SRCS ${srcs_common}
                       INCLUDE_DIRS ${includes}
                       PRIV_REQUIRES ${priv_requires})