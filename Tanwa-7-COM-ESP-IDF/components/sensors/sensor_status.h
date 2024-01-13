///===-----------------------------------------------------------------------------------------===//
///
/// Copyright (c) PWr in Space. All rights reserved.
/// Created: 13.01.2024 by Michał Kos
///
///===-----------------------------------------------------------------------------------------===//
///
/// \file
/// This file contains declaration of unified return codes for sensors drivers.
///===-----------------------------------------------------------------------------------------===//

#ifndef PWRINSPACE_SENSOR_STATUS_H_
#define PWRINSPACE_SENSOR_STATUS_H_

typedef enum {
    SENSOR_OK = 0,
    SENSOR_FAIL,
    SENSOR_INIT_ERR,
    SENSOR_READ_ERR,
    SENSOR_WRITE_ERR,
    SENSOR_CONFIG_ERR,
    SENSOR_TIMEOUT_ERR,
} sensor_status_t;

#endif /* PWRINSPACE_SENSOR_STATUS_H_ */