#include <iostream>
#include <iomanip>
#include <cstdint>
#include <cstring>
#include <string>
#include "../../../core/cipher/trivium/trivium.cpp"

#ifndef CHECK_IMPLEMENTATION_SNOW
#define CHECK_IMPLEMENTATION_SNOW
#include <iostream>
#include <iomanip>
#include <vector>
#include <bitset>
#include <cassert>

int check_implementation_trivium() {
    // Enable debug mode
    trivium_is_debug_mode = 1;
    // Ejemplo de clave (80 bits) - puede cambiarse
    uint8_t key[10] = {
        0x80, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00
    };

    // Ejemplo de IV (80 bits) - puede cambiarse
    uint8_t iv[10] = {
		0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00
    };

    trivium_ctx_t ctx;

    // Inicializar Trivium con clave de 80 bits (80*8 = 640 bits)
    // y IV de 80 bits
    trivium_init(key, 80, iv, 80, &ctx);

    // Generar y mostrar los primeros 32 bytes del keystream
    const int keystream_bytes = 32;
    std::cout << "Keystream (hex):" << std::endl;

    for (int i = 0; i < keystream_bytes; ++i) {
        uint8_t byte = trivium_getbyte(&ctx);
        std::cout << std::hex << std::setw(2) << std::setfill('0') 
                  << static_cast<int>(byte) << " ";
        
        // Nueva línea cada 16 bytes para mejor legibilidad
        if ((i + 1) % 16 == 0) {
            std::cout << std::endl;
        }
    }

    std::cout << std::endl;
    return 0;
}

#endif