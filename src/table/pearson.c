#include "table/pearson.h"

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h> //rand

#define PEARSON_LENGTH 256
#define PEARSON_MASK 0xFF

void pearson_init(
    struct pearson_data * pearson_data)
{
    for (size_t i = 0; i < PEARSON_LENGTH; i++) {
        pearson_data->data[i] = (uint8_t)i;
    }
    for (size_t i = 0; i < PEARSON_LENGTH; i++) {
        size_t j = rand() & PEARSON_MASK;
        uint8_t tmp = pearson_data->data[i];
        pearson_data->data[i] = pearson_data->data[j];
        pearson_data->data[j] = tmp;
    }
}

uint8_t pearson(
    const struct pearson_data * pearson_data,
    size_t nth,
    const uint8_t * data,
    size_t data_length)
{
    uint8_t x = pearson_data->data[(data[0] + nth) & PEARSON_MASK];
    for (size_t dn = 1; dn < data_length; dn++) {
        x = pearson_data->data[x ^ data[dn]];
    }
    return x;
}

