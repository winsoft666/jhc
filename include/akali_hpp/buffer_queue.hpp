/*******************************************************************************
*    Copyright (C) <2022>  <winsoft666@outlook.com>.
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

#ifndef AKALI_BUFFER_QUEUE_HPP__
#define AKALI_BUFFER_QUEUE_HPP__

#include <mutex>
#include <string>
#include <stdlib.h>
#include "akali_hpp/macros.hpp"

namespace akali_hpp {
typedef struct QueueElem {
    void* dataStartAddress;  // start address of the data that we allocated.
    void* dataReadAddress;   // address of the data in buffer. Next time, we get data from this address.
    size_t size;             // the size of the data.
    struct QueueElem* prev;
    struct QueueElem* next;
} QUEUE_ELEMENT;

class BufferQueue {
   public:
    explicit BufferQueue(const std::string& name = "") {
        queue_name_ = name;
        first_element_ = nullptr;
        last_element_ = nullptr;
        element_num_ = 0;
        total_data_size_ = 0;
    }

    ~BufferQueue() {
        clear();
    }

    std::string getQueueName() const {
        return queue_name_;
    }

    // Push element to queue's front.
    // The queue will allocate buffer to save pData, so caller can free pData after call.
    //
    bool pushElementToFront(void* pData, size_t nDataSize) {
        if (pData == nullptr || nDataSize == 0)
            return false;

        QUEUE_ELEMENT* elem = (QUEUE_ELEMENT*)malloc(sizeof(QUEUE_ELEMENT));

        if (!elem)
            return false;

        void* data = malloc(nDataSize);

        if (!data) {
            free(elem);
            return false;
        }

        {
            std::lock_guard<std::recursive_mutex> lg(queue_mutex_);
            memcpy(data, pData, nDataSize);

            elem->dataReadAddress = data;
            elem->dataStartAddress = data;
            elem->size = nDataSize;

            total_data_size_ += nDataSize;
            element_num_++;

            if (first_element_ == nullptr) {  // Add first element in queue.
                // Now,no element in queue.
                elem->prev = nullptr;
                elem->next = nullptr;
                first_element_ = elem;
                last_element_ = elem;
            }
            else {
                elem->prev = nullptr;
                elem->next = first_element_;
                first_element_->prev = elem;
                first_element_ = elem;
            }
        }

        return true;
    }

    // Push element to queue's last.
    // The queue will allocate buffer to save pData, so caller can free pData after call.
    //
    bool pushElementToLast(void* pData, size_t nDataSize) {
        if (pData == nullptr || nDataSize == 0)
            return false;

        QUEUE_ELEMENT* elem = (QUEUE_ELEMENT*)malloc(sizeof(QUEUE_ELEMENT));

        if (!elem)
            return false;

        void* data = malloc(nDataSize);

        if (!data) {
            free(elem);
            return false;
        }

        {
            std::lock_guard<std::recursive_mutex> lg(queue_mutex_);

            memcpy(data, pData, nDataSize);

            elem->dataReadAddress = data;
            elem->dataStartAddress = data;
            elem->size = nDataSize;

            total_data_size_ += nDataSize;
            element_num_++;

            // Add last element in queue.
            if (last_element_ == nullptr) {
                // Now,no element in queue.
                elem->prev = nullptr;
                elem->next = nullptr;
                first_element_ = elem;
                last_element_ = elem;
            }
            else {
                elem->prev = last_element_;
                elem->next = nullptr;
                last_element_->next = elem;
                last_element_ = elem;
            }
        }

        return true;
    }

    // Pop queue's first element, and copy element's data to pBuffer.
    // Caller need allocate/free pBuffer's memory.
    // Return: actual size of copy into pBuffer
    //
    size_t popElementFromFront(void* pBuffer, size_t nBufferSize) {
        size_t rvalue = 0;

        std::lock_guard<std::recursive_mutex> lg(queue_mutex_);

        if (element_num_ > 0) {
            if (pBuffer != nullptr && nBufferSize > 0) {
                // get smaller value of size.
                const size_t size = (first_element_->size > nBufferSize) ? nBufferSize : first_element_->size;
                if (size > 0)
                    memcpy(pBuffer, first_element_->dataReadAddress, size);
                rvalue = size;
            }

            element_num_--;
            total_data_size_ -= first_element_->size;

            QUEUE_ELEMENT* next = first_element_->next;

            if (next != nullptr) {
                next->prev = nullptr;

                if (first_element_->dataStartAddress)
                    free(first_element_->dataStartAddress);

                free(first_element_);
                first_element_ = next;
            }
            else {
                if (first_element_->dataStartAddress)
                    free(first_element_->dataStartAddress);

                free(first_element_);
                first_element_ = nullptr;
                last_element_ = nullptr;
            }
        }

        return rvalue;
    }

    // Pop queue's last element, and copy element's data to pBuffer.
    // Caller need allocate/free pBuffer's memory.
    // Return: actual size of copy into pBuffer
    //
    size_t popElementFromLast(void* pBuffer, size_t nBufferSize) {
        std::lock_guard<std::recursive_mutex> lg(queue_mutex_);
        size_t rvalue = 0;

        if (element_num_ > 0) {
            if (pBuffer != nullptr && nBufferSize > 0) {
                // get smaller value of size
                const size_t size = (last_element_->size > nBufferSize) ? nBufferSize : last_element_->size;
                if (size > 0)
                    memcpy(pBuffer, last_element_->dataReadAddress, size);
                rvalue = size;
            }

            element_num_--;
            total_data_size_ -= last_element_->size;

            QUEUE_ELEMENT* prev = last_element_->prev;

            if (prev) {
                prev->next = nullptr;

                if (last_element_->dataStartAddress)
                    free(last_element_->dataStartAddress);

                free(last_element_);
                last_element_ = prev;
            }
            else {
                if (last_element_->dataStartAddress)
                    free(last_element_->dataStartAddress);

                free(last_element_);
                first_element_ = nullptr;
                last_element_ = nullptr;
            }
        }

        return rvalue;
    }

    // Copy data of the first element to pBuffer.
    // Caller need allocate/free pBuffer's memory.
    // Return: actual size of copy into pBuffer.
    //
    size_t getDataFromFrontElement(void* pBuffer, size_t nBufferSize) {
        std::lock_guard<std::recursive_mutex> lg(queue_mutex_);
        size_t rvalue = 0;

        if (element_num_ > 0) {
            if (pBuffer == nullptr || nBufferSize == 0) {
                rvalue = 0;
            }
            else {
                // get smaller value of size.
                const size_t size = (first_element_->size > nBufferSize) ? nBufferSize : first_element_->size;
                memcpy(pBuffer, first_element_->dataReadAddress, size);
                rvalue = size;
            }
        }
        else {
            rvalue = 0;
        }

        return rvalue;
    }

    // Copy data of the last element to pBuffer.
    // Caller need allocate/free pBuffer's memory.
    // Return: the actual size that be copied into pBuffer.
    //
    size_t getDataFromLastElement(void* pBuffer, size_t nBufferSize) {
        std::lock_guard<std::recursive_mutex> lg(queue_mutex_);
        size_t rvalue = 0;

        if (element_num_ != 0) {
            if (pBuffer == nullptr || nBufferSize == 0) {
                rvalue = 0;
            }
            else {
                // get smaller value of size
                const size_t size = (last_element_->size > nBufferSize) ? nBufferSize : last_element_->size;
                memcpy(pBuffer, last_element_->dataReadAddress, size);
                rvalue = size;
            }
        }
        else {
            rvalue = 0;
        }

        return rvalue;
    }

    // Clear and free all elements.
    // Return: the total number that be freed elements.
    //
    size_t clear() {
        std::lock_guard<std::recursive_mutex> lg(queue_mutex_);
        const size_t rvalue = element_num_;

        if (element_num_ > 0) {  // free memory.
            QUEUE_ELEMENT* elem = first_element_;
            QUEUE_ELEMENT* next = first_element_;

            while (next) {
                if (next->dataStartAddress)
                    free(next->dataStartAddress);

                next = next->next;
                free(elem);
                elem = next;
            }
        }

        first_element_ = nullptr;
        last_element_ = nullptr;
        element_num_ = 0;
        total_data_size_ = 0;

        return rvalue;
    }

    size_t getElementCount() {
        std::lock_guard<std::recursive_mutex> lg(queue_mutex_);
        return element_num_;
    }

    size_t getTotalDataSize() {
        std::lock_guard<std::recursive_mutex> lg(queue_mutex_);
        return total_data_size_;
    }

    size_t popDataCrossElement(void* pOutputBuffer, size_t nBytesToRead, size_t* pBufferIsThrown) {
        std::lock_guard<std::recursive_mutex> lg(queue_mutex_);
        size_t nOutBufferNum = 0;
        size_t rvalue = 0;
        size_t nBytesRead = 0;  // how much bytes has been read.
        size_t nByteNeed = nBytesToRead;
        char* pBuffer = (char*)pOutputBuffer;

        if (element_num_ != 0 && total_data_size_ > 0 && nBytesToRead > 0) {
            while (true) {
                if (first_element_->size >= nByteNeed) {  // we have enough data.
                    memcpy(pBuffer, first_element_->dataReadAddress, nByteNeed);

                    nBytesRead += nByteNeed;
                    first_element_->size -= nByteNeed;
                    total_data_size_ -= nByteNeed;

                    // check if buffer is empty.
                    if (first_element_->size == 0) {
                        // remove this element from queue.
                        nOutBufferNum++;
                        popElementFromFront(nullptr, 0);
                    }
                    else {
                        // element isn't empty, but we have removed some data from
                        // element.
                        first_element_->dataReadAddress =
                            (char*)first_element_->dataReadAddress + nByteNeed;
                    }

                    nByteNeed = 0;
                }
                else {
                    memcpy(pBuffer, first_element_->dataReadAddress, first_element_->size);

                    nBytesRead += first_element_->size;
                    pBuffer += first_element_->size;
                    nByteNeed -= first_element_->size;
                    total_data_size_ -= first_element_->size;
                    first_element_->size = 0;
                    nOutBufferNum++;

                    popElementFromFront(nullptr, 0);
                }

                if (nByteNeed == 0 || element_num_ == 0) {
                    if (pBufferIsThrown)
                        *pBufferIsThrown = nOutBufferNum;

                    rvalue = nBytesRead;
                    break;
                }
            }
        }
        else {
            rvalue = 0;
        }

        return rvalue;
    }

    // Remove data from element(maybe cross elements).
    // Return: the element number that been removed.
    //
    size_t removeData(size_t nBytesToRemove) {
        std::lock_guard<std::recursive_mutex> lg(queue_mutex_);
        size_t rvalue = 1;
        size_t nByteNeed = nBytesToRemove;

        if (element_num_ > 0 && total_data_size_ > 0 && nBytesToRemove > 0) {
            while (total_data_size_ > 0) {
                if (first_element_->size >= nByteNeed) {  // we have enough data.
                    // check if buffer is empty
                    if (first_element_->size == nByteNeed) {  // remove this element from queue
                        popElementFromFront(nullptr, 0);
                    }
                    else {  // element isn't empty, but we have removed some data
                        // from element
                        total_data_size_ -= nByteNeed;
                        first_element_->size -= nByteNeed;
                        first_element_->dataReadAddress = (char*)first_element_->dataReadAddress + nByteNeed;
                    }

                    break;
                }
                else {
                    nByteNeed -= first_element_->size;
                    popElementFromFront(nullptr, 0);
                    rvalue++;
                }
            }
        }
        else {
            rvalue = 0;
        }

        return rvalue;
    }

    size_t getFrontElementDataSize() {
        std::lock_guard<std::recursive_mutex> lg(queue_mutex_);
        return first_element_->size;
    }

    size_t getLastElementDataSize() {
        std::lock_guard<std::recursive_mutex> lg(queue_mutex_);
        return last_element_->size;
    }

    size_t toOneBuffer(char** ppBuf) {
        std::lock_guard<std::recursive_mutex> lg(queue_mutex_);
        if (ppBuf == nullptr)
            return 0;

        const size_t iBufSize = getTotalDataSize();

        *ppBuf = (char*)malloc(iBufSize);

        if (*ppBuf == nullptr)
            return 0;

        QUEUE_ELEMENT* p = first_element_;
        size_t remaind = iBufSize;
        char* pB = *ppBuf;
        while (p && remaind > 0) {
            memcpy(pB, p->dataReadAddress, p->size);
            remaind -= p->size;
            pB += p->size;

            p = p->next;
        }

        return iBufSize;
    }

    size_t toOneBufferWithNullEnding(char** ppBuf) {
        std::lock_guard<std::recursive_mutex> lg(queue_mutex_);
        if (ppBuf == nullptr)
            return 0;

        const size_t iBufSize = getTotalDataSize();

        *ppBuf = (char*)malloc(iBufSize + 1);

        if (*ppBuf == nullptr)
            return 0;

        (*ppBuf)[iBufSize] = 0;

        QUEUE_ELEMENT* p = first_element_;
        size_t remaind = iBufSize;
        char* pB = *ppBuf;
        while (p && remaind > 0) {
            memcpy(pB, p->dataReadAddress, p->size);
            remaind -= p->size;
            pB += p->size;

            p = p->next;
        }

        return iBufSize + 1;
    }

   private:
    QUEUE_ELEMENT* first_element_;
    QUEUE_ELEMENT* last_element_;
    size_t element_num_;
    size_t total_data_size_;
    std::string queue_name_;
    std::recursive_mutex queue_mutex_;

    AKALI_DISALLOW_COPY_AND_ASSIGN(BufferQueue);
};
}  // namespace akali_hpp
#endif  //! AKALI_BUFFER_QUEUE_HPP__