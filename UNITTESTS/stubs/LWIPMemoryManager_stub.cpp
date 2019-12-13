/* Copyright (c) 2019 ARM Limited
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

#include "pbuf.h"
#include "LWIPMemoryManager.h"

net_stack_mem_buf_t *LWIPMemoryManager::alloc_heap(uint32_t size, uint32_t align)
{
    return NULL;
}

net_stack_mem_buf_t *LWIPMemoryManager::alloc_pool(uint32_t size, uint32_t align)
{
    return NULL;
}

uint32_t LWIPMemoryManager::get_pool_alloc_unit(uint32_t align) const
{
    return 0;
}

void LWIPMemoryManager::free(net_stack_mem_buf_t *buf)
{
}

uint32_t LWIPMemoryManager::get_total_len(const net_stack_mem_buf_t *buf) const
{
    return 0;
}

void LWIPMemoryManager::copy(net_stack_mem_buf_t *to_buf, const net_stack_mem_buf_t *from_buf)
{
}

void LWIPMemoryManager::copy_to_buf(net_stack_mem_buf_t *to_buf, const void *ptr, uint32_t len)
{
}

uint32_t LWIPMemoryManager::copy_from_buf(void *ptr, uint32_t len, const net_stack_mem_buf_t *from_buf) const
{
    return 0;
}

void LWIPMemoryManager::cat(net_stack_mem_buf_t *to_buf, net_stack_mem_buf_t *cat_buf)
{
}

net_stack_mem_buf_t *LWIPMemoryManager::get_next(const net_stack_mem_buf_t *buf) const
{
    return NULL;
}

void *LWIPMemoryManager::get_ptr(const net_stack_mem_buf_t *buf) const
{
    return NULL;
}

uint32_t LWIPMemoryManager::get_len(const net_stack_mem_buf_t *buf) const
{
    return 0;
}

void LWIPMemoryManager::set_len(net_stack_mem_buf_t *buf, uint32_t len)
{
}

uint32_t LWIPMemoryManager::count_total_align(uint32_t size, uint32_t align)
{
    return 0;
}

void LWIPMemoryManager::align_memory(struct pbuf *pbuf, uint32_t align)
{
}

void LWIPMemoryManager::set_total_len(struct pbuf *pbuf)
{
}
