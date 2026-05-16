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
 * @brief IR Pioneer scan code representation
 */
typedef struct {
    uint16_t address1;
    uint16_t command1;
    uint16_t address2;
    uint16_t command2;
} ir_pioneer_scan_code_t;

/**
 * @brief Type of IR Pioneer encoder configuration
 */
typedef struct {
    uint32_t resolution; /*!< Encoder resolution, in Hz */
    bool invert_out; // flag, whether output signal is inverted
} ir_pioneer_encoder_config_t;

/**
 * @brief Create RMT encoder for encoding IR Pioneer frame into RMT symbols
 *
 * @param[in] config Encoder configuration
 * @param[out] ret_encoder Returned encoder handle
 * @return
 *      - ESP_ERR_INVALID_ARG for any invalid arguments
 *      - ESP_ERR_NO_MEM out of memory when creating IR NEC encoder
 *      - ESP_OK if creating encoder successfully
 */
esp_err_t rmt_new_ir_pioneer_encoder(const ir_pioneer_encoder_config_t *config, rmt_encoder_handle_t *ret_encoder);

#ifdef __cplusplus
}
#endif