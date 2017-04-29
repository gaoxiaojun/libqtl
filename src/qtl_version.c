/******************************************************************************
 *                         __  __  ____                                       *
 *                        / / / / / __ \                                      *
 *                       / /_/ / / /_/ /          C++                         *
 *                      / __  /  \__, /           Trading                     *
 *                     /_/ /_/  /____/            Framework                   *
 *                                                                            *
 * Copyright (C) 2017   Xiaojun Gao                                           *
 *                                                                            *
 * Distributed under the terms and conditions of the BSD 3-Clause License or  *
 * (at your option) under the terms and conditions of the Boost Software      *
 * License 1.0. See accompanying files LICENSE and LICENSE_ALTERNATIVE.       *
 ******************************************************************************/
#include "qtl_version.h"

#define QTL_STRINGIFY(v) QTL_STRINGIFY_HELPER(v)
#define QTL_STRINGIFY_HELPER(v) #v

#define QTL_VERSION_STRING_BASE  QTL_STRINGIFY(QTL_VERSION_MAJOR) "." \
                                QTL_STRINGIFY(QTL_VERSION_MINOR) "." \
                                QTL_STRINGIFY(QTL_VERSION_PATCH)

#if QTL_VERSION_IS_RELEASE
# define QTL_VERSION_STRING  QTL_VERSION_STRING_BASE
#else
# define QTL_VERSION_STRING  QTL_VERSION_STRING_BASE "-" QTL_VERSION_SUFFIX
#endif


unsigned int qtl_version(void) {
  return QTL_VERSION_HEX;
}


const char* qtl_version_string(void) {
  return QTL_VERSION_STRING;
}
