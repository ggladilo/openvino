# Copyright (C) 2018-2025 Intel Corporation
# SPDX-License-Identifier: Apache-2.0
#

add_subdirectory(logger)
<<<<<<< Updated upstream

if(ENABLE_NPU_PLUGIN_ENGINE)
||||||| Stash base
add_subdirectory(zero)
=======
if(ENABLE_ZEROAPI_BACKEND)
>>>>>>> Stashed changes
    add_subdirectory(zero)
endif()
