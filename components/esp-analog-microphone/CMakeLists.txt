set(srcs_common  "esp_analog_microphone.c")

set(includes "include")

set(priv_requires driver esp_adc)

idf_component_register(SRCS ${srcs_common}
                       INCLUDE_DIRS ${includes}
                       PRIV_REQUIRES ${priv_requires})