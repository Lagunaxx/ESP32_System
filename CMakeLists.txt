#idf_component_register(SRCS "ESP32_SPIDisplay.cpp"
#                    INCLUDE_DIRS "include")

set(CORE_SRCS
  ESP32_System.cpp
  )

# exampl from arduino-esp32 project
#set(LIBRARY_SRCS
#  libraries/ArduinoOTA/src/ArduinoOTA.cpp
#  libraries/AsyncUDP/src/AsyncUDP.cpp
#  libraries/WiFi/src/WiFiUdp.cpp
#  libraries/WiFiProv/src/WiFiProv.cpp
#  libraries/Wire/src/Wire.cpp
#  )
#
#set(BLE_SRCS
#  libraries/BLE/src/BLE2902.cpp
#  libraries/BLE/src/BLE2904.cpp
#  libraries/BLE/src/GeneralUtils.cpp
#  )

set(EXT_SRCS
  Extensions/IO.cpp
  Extensions/ModbusTCP.cpp
  Extensions/ModbusUART.cpp
  Extensions/Network.cpp
  Extensions/Network/wifi/WiFi.cpp
  )
  
set(TMP_SRC
  Extensions/stdin_out.cpp
  )

set(EXT_INCL
  Extensions/Network/wifi
  )
#set(EXT_MODBUS_SRCS
#  $ENV{IDF_PATH}/components/freemodbus/port/port.c
#  )

set(includedirs
#  variants/${IDF_TARGET}/
  include
  Extensions/Include
#  $ENV{IDF_PATH}/tools/cmake/project.cmake
  $ENV{IDF_PATH}/components/freemodbus/common/include
#  $ENV{IDF_PATH}/components/freemodbus/modbus/include
#  $ENV{IDF_PATH}/components/freemodbus/port
#  $ENV{IDF_PATH}/components/freemodbus/tcp_master/modbus_controller
  .
  ${EXT_INCL}
  )
  
#  Extensions/Font/Fonts/Custom
#  Extensions/Font/Fonts/GFXFF
# User_Setups

set(srcs ${CORE_SRCS} ${EXT_SRCS} ${TMP_SRC})

set(prereq
  esp_netif
  )

# ${EXT_MODBUS_SRCS})
#set(priv_includes cores/esp32/libb64)
set(requires arduino)
#set(priv_requires fatfs nvs_flash app_update spiffs bootloader_support openssl bt esp_ipc esp_hid)
idf_component_register(INCLUDE_DIRS ${includedirs} SRCS ${srcs} REQUIRES ${requires} PRIV_REQUIRES ${prereq})
