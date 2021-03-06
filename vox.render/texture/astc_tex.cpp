//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/texture/astc_tex.h"

#include <mutex>

#include "vox.render/error.h"

VKBP_DISABLE_WARNINGS()

#if defined(_WIN32) || defined(_WIN64)
// Windows.h defines IGNORE, so we must #undef it to avoid clashes with astc header
#undef IGNORE
#endif

#include <astc_codec_internals.h>

VKBP_ENABLE_WARNINGS()

#define MAGIC_FILE_CONSTANT 0x5CA1AB13

namespace vox {
BlockDim ToBlockdim(const VkFormat format) {
    switch (format) {
        case VK_FORMAT_ASTC_4x4_UNORM_BLOCK:
        case VK_FORMAT_ASTC_4x4_SRGB_BLOCK:
            return {4, 4, 1};
        case VK_FORMAT_ASTC_5x4_UNORM_BLOCK:
        case VK_FORMAT_ASTC_5x4_SRGB_BLOCK:
            return {5, 4, 1};
        case VK_FORMAT_ASTC_5x5_UNORM_BLOCK:
        case VK_FORMAT_ASTC_5x5_SRGB_BLOCK:
            return {5, 5, 1};
        case VK_FORMAT_ASTC_6x5_UNORM_BLOCK:
        case VK_FORMAT_ASTC_6x5_SRGB_BLOCK:
            return {6, 5, 1};
        case VK_FORMAT_ASTC_6x6_UNORM_BLOCK:
        case VK_FORMAT_ASTC_6x6_SRGB_BLOCK:
            return {6, 6, 1};
        case VK_FORMAT_ASTC_8x5_UNORM_BLOCK:
        case VK_FORMAT_ASTC_8x5_SRGB_BLOCK:
            return {8, 5, 1};
        case VK_FORMAT_ASTC_8x6_UNORM_BLOCK:
        case VK_FORMAT_ASTC_8x6_SRGB_BLOCK:
            return {8, 6, 1};
        case VK_FORMAT_ASTC_8x8_UNORM_BLOCK:
        case VK_FORMAT_ASTC_8x8_SRGB_BLOCK:
            return {8, 8, 1};
        case VK_FORMAT_ASTC_10x5_UNORM_BLOCK:
        case VK_FORMAT_ASTC_10x5_SRGB_BLOCK:
            return {10, 5, 1};
        case VK_FORMAT_ASTC_10x6_UNORM_BLOCK:
        case VK_FORMAT_ASTC_10x6_SRGB_BLOCK:
            return {10, 6, 1};
        case VK_FORMAT_ASTC_10x8_UNORM_BLOCK:
        case VK_FORMAT_ASTC_10x8_SRGB_BLOCK:
            return {10, 8, 1};
        case VK_FORMAT_ASTC_10x10_UNORM_BLOCK:
        case VK_FORMAT_ASTC_10x10_SRGB_BLOCK:
            return {10, 10, 1};
        case VK_FORMAT_ASTC_12x10_UNORM_BLOCK:
        case VK_FORMAT_ASTC_12x10_SRGB_BLOCK:
            return {12, 10, 1};
        case VK_FORMAT_ASTC_12x12_UNORM_BLOCK:
        case VK_FORMAT_ASTC_12x12_SRGB_BLOCK:
            return {12, 12, 1};
        default:
            throw std::runtime_error{"Invalid astc format"};
    }
}

struct AstcHeader {
    uint8_t magic[4];
    uint8_t blockdim_x;
    uint8_t blockdim_y;
    uint8_t blockdim_z;
    uint8_t xsize[3];  // x-size = xsize[0] + xsize[1] + xsize[2]
    uint8_t ysize[3];  // x-size, y-size and z-size are given in texels;
    uint8_t zsize[3];  // block count is inferred
};

void Astc::Init() {
    // Initializes ASTC library
    static bool initialized{false};
    static std::mutex initialization;
    std::unique_lock<std::mutex> lock{initialization};
    if (!initialized) {
        // Init stuff
        prepare_angular_tables();
        build_quantization_mode_table();
        initialized = true;
    }
}

void Astc::Decode(BlockDim blockdim, VkExtent3D extent, const uint8_t *data) {
    // Actual decoding
    astc_decode_mode decode_mode = DECODE_LDR_SRGB;
    uint32_t bitness = 8;
    swizzlepattern swz_decode = {0, 1, 2, 3};

    int xdim = blockdim.x;
    int ydim = blockdim.y;
    int zdim = blockdim.z;

    if ((xdim < 3 || xdim > 6 || ydim < 3 || ydim > 6 || zdim < 3 || zdim > 6) &&
        (xdim < 4 || xdim == 7 || xdim == 9 || xdim == 11 || xdim > 12 || ydim < 4 || ydim == 7 || ydim == 9 ||
         ydim == 11 || ydim > 12 || zdim != 1)) {
        throw std::runtime_error{"Error reading astc: invalid block"};
    }

    int xsize = extent.width;
    int ysize = extent.height;
    int zsize = extent.depth;

    if (xsize == 0 || ysize == 0 || zsize == 0) {
        throw std::runtime_error{"Error reading astc: invalid size"};
    }

    int xblocks = (xsize + xdim - 1) / xdim;
    int yblocks = (ysize + ydim - 1) / ydim;
    int zblocks = (zsize + zdim - 1) / zdim;

    auto astc_image = allocate_image(bitness, xsize, ysize, zsize, 0);
    initialize_image(astc_image);

    imageblock pb{};
    for (int z = 0; z < zblocks; z++) {
        for (int y = 0; y < yblocks; y++) {
            for (int x = 0; x < xblocks; x++) {
                int offset = (((z * yblocks + y) * xblocks) + x) * 16;
                const uint8_t *bp = data + offset;

                physical_compressed_block pcb = *reinterpret_cast<const physical_compressed_block *>(bp);
                symbolic_compressed_block scb{};

                physical_to_symbolic(xdim, ydim, zdim, pcb, &scb);
                decompress_symbolic_block(decode_mode, xdim, ydim, zdim, x * xdim, y * ydim, z * zdim, &scb, &pb);
                write_imageblock(astc_image, &pb, xdim, ydim, zdim, x * xdim, y * ydim, z * zdim, swz_decode);
            }
        }
    }

    SetData(astc_image->imagedata8[0][0], astc_image->xsize * astc_image->ysize * astc_image->zsize * 4);
    SetFormat(VK_FORMAT_R8G8B8A8_SRGB);
    SetWidth(static_cast<uint32_t>(astc_image->xsize));
    SetHeight(static_cast<uint32_t>(astc_image->ysize));
    SetDepth(static_cast<uint32_t>(astc_image->zsize));

    destroy_image(astc_image);
}

Astc::Astc(const Texture &image) : Texture{image.name_} {
    Init();
    Decode(ToBlockdim(image.GetFormat()), image.GetExtent(), image.GetData().data());
}

Astc::Astc(const std::string &name, const std::vector<uint8_t> &data) : Texture{name} {
    Init();

    // Read header
    if (data.size() < sizeof(AstcHeader)) {
        throw std::runtime_error{"Error reading astc: invalid memory"};
    }
    AstcHeader header{};
    std::memcpy(&header, data.data(), sizeof(AstcHeader));
    uint32_t magicval = header.magic[0] + 256 * static_cast<uint32_t>(header.magic[1]) +
                        65536 * static_cast<uint32_t>(header.magic[2]) +
                        16777216 * static_cast<uint32_t>(header.magic[3]);
    if (magicval != MAGIC_FILE_CONSTANT) {
        throw std::runtime_error{"Error reading astc: invalid magic"};
    }

    BlockDim blockdim = {/* xdim = */ header.blockdim_x,
                         /* ydim = */ header.blockdim_y,
                         /* zdim = */ header.blockdim_z};

    VkExtent3D extent = {/* width  = */
                         static_cast<uint32_t>(header.xsize[0] + 256 * header.xsize[1] + 65536 * header.xsize[2]),
                         /* height = */
                         static_cast<uint32_t>(header.ysize[0] + 256 * header.ysize[1] + 65536 * header.ysize[2]),
                         /* depth  = */
                         static_cast<uint32_t>(header.zsize[0] + 256 * header.zsize[1] + 65536 * header.zsize[2])};

    Decode(blockdim, extent, data.data() + sizeof(AstcHeader));
}

}  // namespace vox
