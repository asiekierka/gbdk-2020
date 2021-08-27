        .include        "global.s"

        .title  "VRAM utilities"
        .module VRAMUtils

        .globl  .set_tile_map_xy

        .area   _HOME

; void set_tile_map(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint8_t *tiles) __z88dk_callee __preserves_regs(iyh, iyl);
_set_tile_map::
        pop hl          ; HL = ret
        pop bc          ; BC = YX
        pop de          ; DE = WH
        ex (sp), hl     ; HL = data
        ex de, hl       ; HL = WH, DE = data

        ld a, b
        ld b, d

        add #.SCREEN_Y_OFS
        ld d, a
        xor a
        FAST_MOD8 d #28
        ld d, a

        ld a, c
        add #.SCREEN_X_OFS
        and #0x1f
        ld c, e
        ld e, a         ; BC = data, DE = YX

        jp .set_tile_map_xy
