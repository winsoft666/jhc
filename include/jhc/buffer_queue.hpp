/*******************************************************************************
*    C++ Common Library
*    ---------------------------------------------------------------------------
*    Copyright (C) 2022 JiangXueqiao <winsoft666@outlook.com>.
*
*    This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

#ifndef JHC_BUFFER_QUEUE_HPP__
#define JHC_BUFFER_QUEUE_HPP__

#include <mutex>
#include <string>
#include <string.h>
#include <stdlib.h>
#include "jhc/macros.hpp"

namespace jhc {
typedef struct QueueElem {
    void* dataStartAddress;  // start address of the data that we allocated.
    void* dataReadAddress;   // address of the data in buffer. Next time, we get data from this address.
    size_t size;             // the size of the data.
    struct QueueElem* prev;
    struct QueueElem* next;
} QUEUE_ELEMENT;

class BufferQueue {
   public:
    JHC_DISALLOW_COPY_MOVE(BufferQueue);
    explicit BufferQueue(const std::string& name = "");
    ~BufferQueue();

    std::string getQueueName() const;

    // Push element to queue's front.
    // The queue will allocate buffer to save pData, so caller can free pData after call.
    //
    bool pushElementToFront(void* pData, size_t nDataSize);

    // Push element to queue's last.
    // The queue will allocate buffer to save pData, so caller can free pData after call.
    //
    bool pushElementToLast(void* pData, size_t nDataSize);

    // Pop queue's first element, and copy element's data to pBuffer.
    // Caller need allocate/free pBuffer's memory.
    // Return: actual size of copy into pBuffer
    //
    size_t popElementFromFront(void* pBuffer, size_t nBufferSize);

    // Pop queue's last element, and copy element's data to pBuffer.
    // Caller need allocate/free pBuffer's memory.
    // Return: actual size of copy into pBuffer
    //
    size_t popElementFromLast(void* pBuffer, size_t nBufferSize);

    // Copy data of the first element to pBuffer.
    // Caller need allocate/free pBuffer's memory.
    // Return: actual size of copy into pBuffer.
    //
    size_t getDataFromFrontElement(void* pBuffer, size_t nBufferSize);

    // Copy data of the last element to pBuffer.
    // Caller need allocate/free pBuffer's memory.
    // Return: the actual size that be copied into pBuffer.
    //
    size_t getDataFromLastElement(void* pBuffer, size_t nBufferSize);

    // Clear and free all elements.
    // Return: the total number that be freed elements.
    //
    size_t clear();

    size_t getElementCount();

    size_t getTotalDataSize();

    size_t popDataCrossElement(void* pOutputBuffer, size_t nBytesToRead, size_t* pBufferIsThrown);

    // Remove data from element(maybe cross elements).
    // Return: the element number that been removed.
    //
    size_t removeData(size_t nBytesToRemove);

    size_t getFrontElementDataSize();

    size_t getLastElementDataSize();

    size_t toOneBuffer(char** ppBuf);

    size_t toOneBufferWithNullEnding(char** ppBuf);

   private:
    QUEUE_ELEMENT* first_element_;
    QUEUE_ELEMENT* last_element_;
    size_t element_num_;
    size_t total_data_size_;
    std::string queue_name_;
    std::recursive_mutex queue_mutex_;
};
}  // namespace jhc

#ifndef JHC_NOT_HEADER_ONLY
#include "impl/buffer_queue.cc"
#endif

#endif  //! JHC_BUFFER_QUEUE_HPP__