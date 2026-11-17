#include <stdint.h>
#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#endif

#ifndef EF_FAULT
#define EF_FAULT 0
#endif

#if EF_FAULT < 0 || EF_FAULT > 3
#error "EF_FAULT must be between 0 and 3"
#endif

enum {
    HEADER_SIZE = 6,
    MAX_PAYLOAD = 64,
    MAX_FRAME = HEADER_SIZE + MAX_PAYLOAD,
    RESPONSE_HEADER = 3
};

enum status {
    OK,
    INVALID_FRAME,
    UNSUPPORTED_VERSION,
    UNSUPPORTED_COMMAND,
    INVALID_PAYLOAD
};

struct response {
    enum status status;
    size_t length;
    uint8_t data[MAX_PAYLOAD];
};

static struct response handle_request(const uint8_t *frame, size_t size)
{
    if (size < HEADER_SIZE || size > MAX_FRAME ||
        frame[0] != 0x45 || frame[1] != 0x46) {
        return (struct response){.status = INVALID_FRAME};
    }

    /* The declared length is sender-controlled. Check it before using payload bytes. */
    const size_t length = frame[4] | ((size_t)frame[5] << 8);
    const size_t available = size - HEADER_SIZE;
#if EF_FAULT == 3
    if (length > MAX_PAYLOAD || length > available) {
#else
    if (length > MAX_PAYLOAD || length != available) {
#endif
        return (struct response){.status = INVALID_FRAME};
    }
    if (frame[2] != 1) {
        return (struct response){.status = UNSUPPORTED_VERSION};
    }

    const uint8_t *payload = frame + HEADER_SIZE;
    switch (frame[3]) {
    case 1: /* PING */
        return (struct response){.status = length == 0 ? OK : INVALID_PAYLOAD};
    case 2: { /* READ_SENSOR */
        if (length != 1) {
            return (struct response){.status = INVALID_PAYLOAD};
        }
        uint8_t sensor = payload[0];
#if EF_FAULT == 2
        sensor &= 1;
#endif
        if (sensor > 1) {
            return (struct response){.status = INVALID_PAYLOAD};
        }
        return (struct response){.status = OK, .length = 2,
                                 .data = {sensor == 0 ? 100 : 200, 0}};
    }
    case 3: { /* ECHO */
        size_t copy_length = length;
#if EF_FAULT == 1
        if (copy_length == MAX_PAYLOAD) {
            --copy_length;
        }
#endif
        struct response result = {.status = OK, .length = copy_length};
        memcpy(result.data, payload, copy_length);
        return result;
    }
    default:
        return (struct response){.status = UNSUPPORTED_COMMAND};
    }
}

int main(void)
{
#ifdef _WIN32
    if (_setmode(_fileno(stdin), _O_BINARY) == -1 ||
        _setmode(_fileno(stdout), _O_BINARY) == -1) {
        fputs("could not set binary stream mode\n", stderr);
        return 1;
    }
#endif

    /* Reading one extra byte is enough to detect an oversized request. */
    uint8_t frame[MAX_FRAME + 1];
    const size_t count = fread(frame, 1, sizeof frame, stdin);
    if (ferror(stdin)) {
        fputs("could not read request\n", stderr);
        return 1;
    }

    const struct response result = handle_request(frame, count);
    uint8_t wire[RESPONSE_HEADER + MAX_PAYLOAD] = {
        (uint8_t)result.status, (uint8_t)result.length,
        (uint8_t)(result.length >> 8)
    };
    memcpy(wire + RESPONSE_HEADER, result.data, result.length);
    const size_t response_size = RESPONSE_HEADER + result.length;
    if (fwrite(wire, 1, response_size, stdout) != response_size ||
        fflush(stdout) == EOF) {
        fputs("could not write response\n", stderr);
        return 1;
    }
    return 0;
}
