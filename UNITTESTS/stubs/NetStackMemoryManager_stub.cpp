/*
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "NetStackMemoryManager.h"

void NetStackMemoryManager::copy_to_buf(net_stack_mem_buf_t *to_buf, const void *ptr, uint32_t len)
{
}

uint32_t NetStackMemoryManager::copy_from_buf(void *ptr, uint32_t len, const net_stack_mem_buf_t *from_buf) const
{

    return 0;
}

