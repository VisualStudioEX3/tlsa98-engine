//! SYSTEM.DLL - System library for DIV Games Studio 2.
//! @copyright TLSA98 Engine (C) VisualStudioEX3, José Miguel Sánchez Fernández - 2022, 2023
//! @copyright DIV Games Studio 2 (C) Hammer Technologies, Daniel Navarro Medrano - 1998, 1999
/*!
    Ring buffer.
*/

#include "..\H\SYSTEM\RINGBUFF.H"

int getRingBufferCapacity(const ring_buffer_t *buffer)
{
    return getListCapacity(buffer->list);
}

int getRingBufferCount(const ring_buffer_t *buffer)
{
    return getListCount(buffer->list);
}

bool isRingBufferEmpty(const ring_buffer_t *buffer)
{
    return isListEmpty(buffer->list);
}

bool isRingBufferFull(const ring_buffer_t *buffer)
{
    return isListFull(buffer->list);
}

ring_buffer_t *createRingBuffer(uint capacity, size_t itemSize)
{
    ring_buffer_t *buffer = NULL;

    if (capacity > 0 && itemSize > 0)
    {
        buffer = (ring_buffer_t *)malloc(sizeof(ring_buffer_t));

        if (buffer)
        {
            buffer->list = createList(capacity, itemSize);
            buffer->firstIndex = 0;
            buffer->lastIndex = 0;

            if (buffer->list == NULL)
            {
                free(buffer);
                buffer = NULL;
            }
        }
    }

    return buffer;
}

void clearRingBuffer(ring_buffer_t *buffer)
{
    clearRingBufferEx(buffer, NULL);
}

void clearRingBufferEx(ring_buffer_t *buffer, foreach_ring_buffer_item_callback onItem)
{
    if (buffer)
    {
        forEachRingBuffer(buffer, onItem);
        buffer->firstIndex = 0;
        buffer->lastIndex = 0;
    }
}

void destroyRingBuffer(ring_buffer_t *buffer)
{
    destroyRingBufferEx(buffer, NULL);
}

void destroyRingBufferEx(ring_buffer_t *buffer, foreach_ring_buffer_item_callback onItem)
{
    if (buffer)
    {
        clearRingBufferEx(buffer, onItem);
        destroyList(buffer->list);
        free(buffer);
    }
}

void *createRingBufferItem(ring_buffer_t *buffer)
{
    return createRingBufferItemEx(buffer, NULL);
}

void *createRingBufferItemEx(ring_buffer_t *buffer, create_ring_buffer_item_callback onBeforeCreate, ...)
{
    va_list args;

    va_start(args, onBeforeCreate);

    void *ret = createRingBufferItemExArgs(buffer, onBeforeCreate, args);

    va_end(args);

    return ret;
}

void *createRingBufferItemExArgs(ring_buffer_t *buffer, create_ring_buffer_item_callback onBeforeCreate, va_list args)
{
    void *item = NULL;
    bool canCreate = true;
    va_list copyArgs;

    if (buffer)
    {
        if (onBeforeCreate)
        {
            va_copy(copyArgs, args);
            canCreate = onBeforeCreate(buffer, copyArgs);
        }

        if (canCreate)
        {
            if (isRingBufferFull(buffer))
            {
                item = getListItem(buffer->list, buffer->lastIndex);

                uint capacity = getRingBufferCapacity(buffer);

                if (++buffer->firstIndex == capacity)
                    buffer->firstIndex = 0;

                if (++buffer->lastIndex == capacity)
                    buffer->lastIndex = 0;
            }
            else
            {
                item = createListItem(buffer->list);
                buffer->lastIndex++;
            }
        }
    }

    return item;
}

void *getRingBufferItem(const ring_buffer_t *buffer, int index)
{
    int count = getRingBufferCount(buffer);
    int bufferIndex = buffer->firstIndex + index;

    if (bufferIndex >= count)
        bufferIndex = buffer->firstIndex - (bufferIndex - count);

    void *item = getListItem(buffer->list, bufferIndex);

    return item;
}

void forEachRingBuffer(const ring_buffer_t *buffer, foreach_ring_buffer_item_callback onItem, ...)
{
    va_list args;

    va_start(args, onItem);
    forEachRingBufferArgs(buffer, onItem, args);
    va_end(args);
}

void forEachRingBufferArgs(const ring_buffer_t *buffer, foreach_ring_buffer_item_callback onItem, va_list args)
{
    va_list copyArgs;

    if (!isRingBufferEmpty(buffer) && onItem)
    {
        int count = getRingBufferCount(buffer);
        int bufferIndex = buffer->firstIndex;

        for (int i = 0; i < count; i++)
        {
            va_copy(copyArgs, args);
            onItem(i, getListItem(buffer->list, bufferIndex), copyArgs);

            if (++bufferIndex == count)
                bufferIndex = 0;
        }
    }
}
