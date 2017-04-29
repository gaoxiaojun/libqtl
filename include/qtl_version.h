/******************************************************************************
 * Quantitative Trading Library                                               *
 *                                                                            *
 * Copyright (C) 2017 Xiaojun Gao                                             *
 *                                                                            *
 * Distributed under the terms and conditions of the BSD 3-Clause License or  *
 * (at your option) under the terms and conditions of the Boost Software      *
 * License 1.0. See accompanying files LICENSE and LICENSE_ALTERNATIVE.       *
 ******************************************************************************/

#ifndef QTL_VERSION_H
#define QTL_VERSION_H

// Quantitative Trading Library

#define QTL_VERSION_MAJOR 1
#define QTL_VERSION_MINOR 11
#define QTL_VERSION_PATCH 1
#define QTL_VERSION_IS_RELEASE 0
#define QTL_VERSION_SUFFIX "dev"

#define QTL_VERSION_HEX  ((QTL_VERSION_MAJOR << 16) | \
                         (QTL_VERSION_MINOR <<  8) | \
                         (QTL_VERSION_PATCH))

#endif // QTL_VERSION_H
