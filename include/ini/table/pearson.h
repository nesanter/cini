/* Copyright © 2019 Noah Santer <personal@mail.mossy-tech.com>
 *
 * This file is part of cini.
 *
 * cini is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * cini is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with cini.  If not, see <https://www.gnu.org/licenses/>.
 */
#ifndef PEARSON_H
#define PEARSON_H

#include <stdint.h>

struct pearson_data {
    uint8_t data[256];
};

void pearson_init(
    struct pearson_data * pearson_data);

uint8_t pearson(
    const struct pearson_data * pearson_data,
    size_t nth,
    const uint8_t * data,
    size_t data_length)

#endif /* PEARSON_H */
