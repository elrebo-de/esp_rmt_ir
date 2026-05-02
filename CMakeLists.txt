idf_component_register( SRCS "rmt_ir.cpp"
                             "nec_protocol.cpp"
                             "ir_nec_encoder.c"
                             "panasonic_protocol.cpp"
                             "ir_panasonic_encoder.c"
                        INCLUDE_DIRS "include"
                        REQUIRES "driver"
                                 "esp_timer"
                                 "esp_event"
                                 "esp_driver_rmt"
                      )

