/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include "driver/rmt_encoder.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief IR PANASONIC scan code representation
 */
typedef struct {
    uint16_t non_saving_bits_1;
    uint8_t system_code;
    uint8_t address;
    uint8_t command;
    uint8_t checksum; // checksum = system_code XOR address XOR command
} ir_panasonic_scan_code_t;

/**
 * @brief Type of IR PANASONIC encoder configuration
 */
typedef struct {
    uint32_t resolution; /*!< Encoder resolution, in Hz */
    bool invert_out; // flag, whether output signal is inverted
} ir_panasonic_encoder_config_t;

/**
 * @brief Create RMT encoder for encoding IR PANASONIC frame into RMT symbols
 *
 * @param[in] config Encoder configuration
 * @param[out] ret_encoder Returned encoder handle
 * @return
 *      - ESP_ERR_INVALID_ARG for any invalid arguments
 *      - ESP_ERR_NO_MEM out of memory when creating IR NEC encoder
 *      - ESP_OK if creating encoder successfully
 */
esp_err_t rmt_new_ir_panasonic_encoder(const ir_panasonic_encoder_config_t *config, rmt_encoder_handle_t *ret_encoder);

#ifdef __cplusplus
}
#endif