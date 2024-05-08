// This prevents windows build errors regarding the 'fopen' function
// Example: 'fopen': This function or variable may be unsafe.Consider using fopen_s instead.To disable deprecation, use _CRT_SECURE_NO_WARNINGS.See online help for details.
// More info: https://stackoverflow.com/questions/14386/fopen-deprecated-warning
// From bbbbbr: `Looks like some of the benefits are concurrency protection, but that probably isn't an issue for png2asset`
#ifdef _WIN32
#define _CRT_SECURE_NO_DEPRECATE
#endif

#include <vector>
#include <string>
#include <algorithm>
#include <cstring>
#include <set>
#include <stdio.h>
#include <fstream>
#include <cstdint>

#include "lodepng.h"
#include "mttile.h"

#include "cmp_int_color.h"
#include "metasprites.h"

#include "png2asset.h"
#include "png_image.h"
#include "export.h"
#include "image_utils.h"
#include "process_arguments.h"
#include "rgb_to_nes_lut.h"

using namespace std;

enum {
    ZGB_TILES_MODE_NORMAL,
    ZGB_TILES_MODE_TILESONLY
};

static exportOptions_t exportOpt;

static void export_c_palette_data(PNG2AssetData* assetData, FILE* file);
static void export_c_tile_data(PNG2AssetData* assetData, FILE* file);
static void export_c_metasprite_mode(PNG2AssetData* assetData, FILE* file);
static void export_c_map_mode(PNG2AssetData* assetData, FILE* file);

static void export_c_zgb_metasprite_struct(PNG2AssetData* assetData, FILE* file);
static void export_c_zgb_per_tile_palette_ids(PNG2AssetData* assetData, FILE* file);
static void export_c_zgb_tiles_struct(PNG2AssetData* assetData, FILE* file, int output_mode);
static void export_c_zgb_map_struct(PNG2AssetData* assetData, FILE* file);


bool export_c_file( PNG2AssetData* assetData) {

    FILE* file;
    calc_palette_and_tileset_export_size(assetData, &exportOpt);

    file = fopen(assetData->args->output_filename.c_str(), "w");
    if(!file) {
        printf("Error writing file: %s", assetData->args->output_filename.c_str());
        return false;
    }

    if(assetData->args->bank >= 0) fprintf(file, "#pragma bank %d\n\n", assetData->args->bank);

    fprintf(file, "//AUTOGENERATED FILE FROM png2asset\n\n");

    fprintf(file, "#include <stdint.h>\n");
    fprintf(file, "#include <gbdk/platform.h>\n");
    fprintf(file, "#include <gbdk/metasprites.h>\n");

    if (assetData->args->use_structs) {
        fprintf(file, "#include \"TilesInfo.h\"\n");
        if (assetData->args->includedMapOrMetaspriteData) {
            if (assetData->args->export_as_map)
                fprintf(file, "#include \"MapInfo.h\"\n");
            else
                fprintf(file, "#include \"MetaSpriteInfo.h\"\n");
        }
    }

    fprintf(file, "\n");

    fprintf(file, "BANKREF(%s)\n\n", assetData->args->data_name.c_str());

    if (exportOpt.has_palette_data_to_export)
        export_c_palette_data(assetData, file);

    if (assetData->args->includeTileData)
        export_c_tile_data(assetData, file);

    if (assetData->args->includedMapOrMetaspriteData) {

        if (assetData->args->export_as_map) {
            export_c_map_mode(assetData, file);
        }
        else {
            export_c_metasprite_mode(assetData, file);
        }
    }
    else {
        // "-use_structs -tiles_only" case
        if ((assetData->args->includeTileData) && (assetData->args->use_structs)) {
            export_c_zgb_per_tile_palette_ids(assetData, file);
            export_c_zgb_tiles_struct(assetData, file, ZGB_TILES_MODE_TILESONLY);
        }
    }

    fclose(file);
    return true; // success
}


static void export_c_palette_data(PNG2AssetData* assetData, FILE* file) {

    if (!(assetData->args->use_structs) || (assetData->args->includeTileData)) {
        fprintf(file, "const palette_color_t %s_palettes[%d] = {\n", assetData->args->data_name.c_str(), (unsigned int)exportOpt.color_count);
        for(size_t i = exportOpt.color_start / assetData->image.colors_per_pal; i < assetData->image.total_color_count / assetData->image.colors_per_pal; ++i)
        {
            if(i != 0)
                fprintf(file, ",\n");
            fprintf(file, "\t");

            unsigned char* pal_ptr = &assetData->image.palette[i * (assetData->image.colors_per_pal * RGBA32_SZ)];
            for(int c = 0; c < (int)assetData->image.colors_per_pal; ++c, pal_ptr += RGBA32_SZ)
            {
                size_t rgb222 = (((pal_ptr[2] >> 6) & 0x3) << 4) |
                    (((pal_ptr[1] >> 6) & 0x3) << 2) |
                    (((pal_ptr[0] >> 6) & 0x3) << 0);
                if(assetData->args->convert_rgb_to_nes) {
                    fprintf(file, "0x%0X", rgb_to_nes[rgb222]);
                }
                else
                    fprintf(file, "RGB8(%3d,%3d,%3d)", pal_ptr[0], pal_ptr[1], pal_ptr[2]);
                if(c != (int)assetData->image.colors_per_pal - 1)
                    fprintf(file, ", ");
                // Line break every 4 color entries, to keep line width down
                if(((c + 1) % 4) == 0)
                    fprintf(file, "\n\t");
            }
        }
        fprintf(file, "\n};\n");
    }
}


static void export_c_tile_data(PNG2AssetData* assetData, FILE* file) {

    fprintf(file, "\n");
    fprintf(file, "const uint8_t %s_tiles[%d] = {\n", assetData->args->data_name.c_str(), (unsigned int)(exportOpt.tiles_count * assetData->image.tile_w * assetData->image.tile_h * assetData->args->bpp / 8));
    fprintf(file, "\t");
    for(vector< Tile >::iterator it = assetData->tiles.begin() + exportOpt.tiles_start; it != assetData->tiles.end(); ++it)
    {

        int line_break = 1; // Start with 1 to prevent line break on first iteration
        vector< unsigned char > packed_data = (*it).GetPackedData(assetData->args->pack_mode, assetData->image.tile_w, assetData->image.tile_h, assetData->args->bpp);
        for(vector< unsigned char >::iterator it2 = packed_data.begin(); it2 != packed_data.end(); ++it2)
        {
            fprintf(file, "0x%02x", (*it2));
            if((it + 1) != assetData->tiles.end() || (it2 + 1) != packed_data.end())
                fprintf(file, ",");
            // Add a line break after each 8x8 tile
            if(((line_break++) % (8 / assetData->args->bpp)) == 0)
                fprintf(file, "\n\t");
        }

        if((!assetData->args->export_as_map) && (it != assetData->tiles.end()))
            fprintf(file, "\n");
    }
    fprintf(file, "};\n\n");

}


static void export_c_metasprite_mode(PNG2AssetData* assetData, FILE* file) {

    for(vector< MetaSprite >::iterator it = assetData->sprites.begin(); it != assetData->sprites.end(); ++it)
    {
        fprintf(file, "const metasprite_t %s_metasprite%d[] = {\n", assetData->args->data_name.c_str(), (int)(it - assetData->sprites.begin()));
        for(MetaSprite::iterator it2 = (*it).begin(); it2 != (*it).end(); ++it2)
        {
            int pal_idx  = (*it2).props & 0xF;
            int dmg_pal  = ((*it2).props >> 4) & 1;
            int flip_x   = ((*it2).props >> 5) & 1;
            int flip_y   = ((*it2).props >> 6) & 1;
            int priority = ((*it2).props >> 7) & 1;
            fprintf(file,
                "\tMETASPR_ITEM(%d, %d, %d, S_PAL(%d)%s%s%s%s),\n",
                (*it2).offset_y,
                (*it2).offset_x,
                (*it2).offset_idx,
                pal_idx,
                (dmg_pal)  ? " | S_PALETTE"  : "",
                (flip_x)   ? " | S_FLIPX"    : "",
                (flip_y)   ? " | S_FLIPY"    : "",
                (priority) ? " | S_PRIORITY" : "");
        }
        fprintf(file, "\tMETASPR_TERM\n");
        fprintf(file, "};\n\n");
    }

    fprintf(file, "const metasprite_t* const %s_metasprites[%d] = {\n\t", assetData->args->data_name.c_str(), (unsigned int)(assetData->sprites.size()));
    for(vector< MetaSprite >::iterator it = assetData->sprites.begin(); it != assetData->sprites.end(); ++it)
    {
        fprintf(file, "%s_metasprite%d", assetData->args->data_name.c_str(), (int)(it - assetData->sprites.begin()));
        if(it + 1 != assetData->sprites.end())
            fprintf(file, ", ");
    }
    fprintf(file, "\n};\n");

    if(assetData->args->use_structs)
        export_c_zgb_metasprite_struct(assetData, file);

}

static void export_c_zgb_metasprite_struct(PNG2AssetData* assetData, FILE* file) {
    // Export ZGB Metasprite Struct
    fprintf(file, "\n");
    fprintf(file, "const struct MetaSpriteInfo %s = {\n",   assetData->args->data_name.c_str());
    fprintf(file, "\t.width=%d,"                   " // width\n",        (unsigned int)assetData->args->pivot.width);
    fprintf(file, "\t.height=%d,"                  " // height\n",       (unsigned int)assetData->args->pivot.height);
    fprintf(file, "\t.num_tiles=%d,"               " // num tiles\n",    (unsigned int)assetData->tiles.size() * (assetData->image.tile_h >> 3));
    fprintf(file, "\t.data=%s_tiles,"              " // tiles\n",        assetData->args->data_name.c_str());
    fprintf(file, "\t.num_palettes=%d,"            " // num palettes\n", (unsigned int)(assetData->image.total_color_count / assetData->image.colors_per_pal));
    fprintf(file, "\t.palettes=%s_palettes,"       " // CGB palette\n",  assetData->args->data_name.c_str());
    fprintf(file, "\t.num_sprites=%d,"             " // num sprites\n",  (unsigned int)assetData->sprites.size());
    fprintf(file, "\t.metasprites=%s_metasprites," " // metasprites\n",  assetData->args->data_name.c_str());
    fprintf(file, "};\n");
}


static void export_c_map_mode(PNG2AssetData* assetData, FILE* file) {

    // ZGB export mode
    if(assetData->args->use_structs)
    {
        // For ZGB Structs with source_tileset enabled, the primary tile data points to the EXTERNAL source tileset
        // so add the required externs used to reference them in the struct
        if( (assetData->args->has_source_tilesets)) {
            fprintf(file, "\n");
            fprintf(file, "BANKREF_EXTERN(%s)\n", extract_name(assetData->args->source_tilesets[0]).c_str());
            fprintf(file, "extern const struct TilesInfo %s;\n", extract_name(assetData->args->source_tilesets[0]).c_str());
        }

        // Map's own tile data, if any present
        if(assetData->args->includeTileData) {
            export_c_zgb_per_tile_palette_ids(assetData, file);
            export_c_zgb_tiles_struct(assetData, file, ZGB_TILES_MODE_NORMAL);
        }
    }

    //Export map
    fprintf(file, "\n");
    fprintf(file, "const unsigned char %s_map[%d] = {\n", assetData->args->data_name.c_str(), (unsigned int)(assetData->map.size()));
    // TODO: These hardwired "/ 8" should be using " / assetData->image.tile_w" and "_h"
    // TODO: Should also be converted to using args.map_entry_size_bytes
    size_t line_size = assetData->map.size() / (assetData->image.h / 8);
    if(assetData->args->output_transposed) {

        for(size_t i = 0; i < line_size; ++i)
        {
            fprintf(file, "\t");
            for(size_t j = 0; j < assetData->image.h / 8; ++j)
            {
                fprintf(file, "0x%02x,", assetData->map[j * line_size + i]);
            }
            fprintf(file, "\n");
        }
    }
    else {

        for(size_t j = 0; j < assetData->image.h / 8; ++j)
        {
            fprintf(file, "\t");
            for(size_t i = 0; i < line_size; ++i)
            {
                fprintf(file, "0x%02x,", assetData->map[j * line_size + i]);
            }
            fprintf(file, "\n");
        }
    }
    fprintf(file, "};\n");


    //Export map attributes (if any)
    if(assetData->args->use_map_attributes && assetData->map_attributes.size())
    {
        fprintf(file, "\n");
        fprintf(file, "const unsigned char %s_map_attributes[%d] = {\n", assetData->args->data_name.c_str(), (unsigned int)(assetData->map_attributes.size()));
        if(assetData->args->output_transposed) {
            for(size_t i = 0; i < assetData->args->map_attributes_packed_size.width; ++i)
            {
                fprintf(file, "\t");
                for(size_t j = 0; j < assetData->args->map_attributes_packed_size.height; ++j)
                {
                    fprintf(file, "0x%02x,", assetData->map_attributes[j * assetData->args->map_attributes_packed_size.width + i]);
                }
                fprintf(file, "\n");
            }
        }
        else {
            for(size_t j = 0; j < assetData->args->map_attributes_packed_size.height; ++j)
            {
                fprintf(file, "\t");
                for(size_t i = 0; i < assetData->args->map_attributes_packed_size.width; ++i)
                {
                    fprintf(file, "0x%02x,", assetData->map_attributes[j * assetData->args->map_attributes_packed_size.width + i]);
                }
                fprintf(file, "\n");
            }
        }

        fprintf(file, "};\n");
    }

    if(assetData->args->use_structs)
        export_c_zgb_map_struct(assetData, file);
}


static void export_c_zgb_per_tile_palette_ids(PNG2AssetData* assetData, FILE* file) {
    //Export tiles pals (if any)
    if(!assetData->args->use_map_attributes)
    {
        fprintf(file, "\n");
        fprintf(file, "const uint8_t %s_tile_pals[%d] = {\n\t", assetData->args->data_name.c_str(), (unsigned int)exportOpt.tiles_count);
        for(vector< Tile >::iterator it = assetData->tiles.begin() + exportOpt.tiles_start; it != assetData->tiles.end(); ++it)
        {
            fprintf(file, "%d", it->pal);
            if (it + 1 != assetData->tiles.end())
                fprintf(file, ", ");
        }
        fprintf(file, "\n};\n");
    }
}


static void export_c_zgb_tiles_struct(PNG2AssetData* assetData, FILE* file, int output_mode) {
    // Export Tiles Info
    fprintf(file, "\n");
    // ZGB Tiles-only mode doesn't append "_tiles_info" to the struct name and lacks a BANKREF output.
    if (output_mode == ZGB_TILES_MODE_TILESONLY) {
        fprintf(file, "const struct TilesInfo %s = {\n",                   assetData->args->data_name.c_str());
    }
    else { // implied (output_mode == ZGB_TILES_MODE_NORMAL)
        fprintf(file, "BANKREF(%s_tiles_info)\n", assetData->args->data_name.c_str());
        fprintf(file, "const struct TilesInfo %s_tiles_info = {\n",        assetData->args->data_name.c_str());
    }

    fprintf(file, "\t.num_frames=%d,"            " // num tiles\n",    (unsigned int)assetData->tiles.size() * (assetData->image.tile_h >> 3));
    fprintf(file, "\t.data=%s_tiles,"            " // tiles\n",        assetData->args->data_name.c_str());
    fprintf(file, "\t.num_pals=%d,"              " // num palettes\n", (unsigned int)(exportOpt.color_count / assetData->image.colors_per_pal));
    fprintf(file, "\t.pals=%s_palettes,"         " // palettes\n",     assetData->args->data_name.c_str());

    // This var references the output of export_c_zgb_per_tile_palette_ids()
    if(!assetData->args->use_map_attributes)
        fprintf(file, "\t.color_data=%s_tile_pals, // tile palettes\n", assetData->args->data_name.c_str());
    else
        fprintf(file, "\t.color_data=0 "         " // tile palettes\n");
    fprintf(file, "};\n");
}


// "-use_structs" map output behavior
//
//   "-source_tileset" NOT enabled:
//     - tile data -> TileInfo struct, point .tiles to it
//     - .tiles points to TileInfo struct, .extra_tiles is NULL
//     - only map tile palettes *not* found in the source tileset will be emitted for .num_pals/.pals
//
//   "-source_tileset" IS enabled
//     - .tiles -> the external source tileset
//     - .extra_tiles -> map TileInfo
//       - contains unique tiles not found in source tileset (not in entity tiles)
//         - if none of those tiles were found then
//           - map TileInfo will not be emitted
//           - .extra_tiles == NULL then
//     - *all* map tile palettes will be emitted for .num_pals/.pals (those from map and source tileset)
//
static void export_c_zgb_map_struct(PNG2AssetData* assetData, FILE* file) {
    //Export Map Info
    fprintf(file, "\n");

    if(assetData->args->includeTileData) {
        fprintf(file, "BANKREF_EXTERN(%s_tiles_info)\n", assetData->args->data_name.c_str());
    }

    fprintf(file, "const struct MapInfo %s = {\n",                       assetData->args->data_name.c_str());
    fprintf(file, "\t.data=%s_map,"                      " // map\n",    assetData->args->data_name.c_str());
    fprintf(file, "\t.width=%d,"                         " // width\n",  assetData->image.w >> 3);
    fprintf(file, "\t.height=%d,"                        " // height\n", assetData->image.h >> 3);

    if(assetData->args->use_map_attributes && assetData->map_attributes.size())
        fprintf(file, "\t.attributes=%s_map_attributes," " // map attributes\n", assetData->args->data_name.c_str());
    else
        fprintf(file, "\t.attributes=0,"                 " // map attributes\n");

    // For ZGB Structs with source_tileset enabled, the tile data points to the EXTERNAL source tileset
    if (assetData->args->has_source_tilesets == true) {
        fprintf(file, "\t.tiles_bank=BANK(%s),"     " // source tiles bank\n", extract_name(assetData->args->source_tilesets[0]).c_str());
        fprintf(file, "\t.tiles=&%s,"               " // source tiles info\n", extract_name(assetData->args->source_tilesets[0]).c_str());

        // If map tiles were found that were not in the source tileset (or entity tileset)
        // then point ".extra_tiles" to them. Otherwise set it to null to indicate none found.
        if (exportOpt.tiles_count > 0) {
            fprintf(file, "\t.extra_tiles=&%s_tiles_info,"        " // pointer to Tilesinfo struct with map tiles not found in the source tileset\n", assetData->args->data_name.c_str());
            fprintf(file, "\t.extra_tiles_bank=BANK(%s_tiles_info), // bank for above Tilesinfo struct\n", assetData->args->data_name.c_str());
        }
        else {
            fprintf(file, "\t.extra_tiles=NULL,"                  " // pointer to Tilesinfo struct with map tiles not found in the source tileset (no extra tiles found)\n");
            fprintf(file, "\t.extra_tiles_bank=0,"                " // bank for above Tilesinfo struct\n");
        }

    }
    else if(assetData->args->includeTileData) {
        fprintf(file, "\t.tiles_bank=BANK(%s_tiles_info), // tiles bank\n", assetData->args->data_name.c_str());
        fprintf(file, "\t.tiles=&%s_tiles_info,"        " // tiles info\n", assetData->args->data_name.c_str());
    }
    fprintf(file, "};\n");
}
