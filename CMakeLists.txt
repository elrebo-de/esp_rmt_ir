idf_component_register( SRCS "rmt_ir.cpp" "ir_nec_encoder.c"
                        INCLUDE_DIRS "include"
                        REQUIRES "driver"
                                 "esp_timer"
                                 "esp_event"
                                 "esp_driver_rmt"
                      )

