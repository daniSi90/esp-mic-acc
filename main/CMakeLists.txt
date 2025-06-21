set(srcs_common  "main.c")

set(includes "")

set(priv_requires "esp_microphone")

idf_component_register(SRCS ${srcs_common}
                       INCLUDE_DIRS ${includes}
                       PRIV_REQUIRES ${priv_requires})
