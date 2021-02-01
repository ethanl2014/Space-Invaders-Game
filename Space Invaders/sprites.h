class Sprite
{
    public:
        size_t width, height;
        uint8_t* data;
};

class SpriteAnimation
{
    public:
        bool loop;
        size_t num_frames;
        size_t frame_duration;
        size_t time;
        Sprite** frames;
};

bool sprite_overlap_check(const Sprite& sp_a, size_t x_a, size_t y_a, const Sprite& sp_b, size_t x_b, size_t y_b)
{
    if (x_a < x_b + sp_b.width && x_a + sp_a.width > x_b && y_a < y_b + sp_b.height && y_a + sp_a.height > y_b)
    {return true;}
    else
    {return false;}
}

void buffer_draw_sprite(Buffer2* buffer2, const Sprite& sprite, size_t x, size_t y, uint32_t color)
{
    for (size_t xi = 0; xi < sprite.width; ++xi)
    {
        for (size_t yi = 0; yi < sprite.height; ++yi)
        {
            if (sprite.data[yi * sprite.width + xi] &&
                (sprite.height - 1 + y - yi) < buffer2->height &&
                (x + xi) < buffer2->width)
            {
                buffer2->data[(sprite.height - 1 + y - yi) * buffer2->width + (x + xi)] = color;
            }
        }
    }
}

void buffer_draw_text(Buffer2* buffer2, const Sprite& text_spritesheet, const char* text, size_t x, size_t y, uint32_t color){
    size_t xp = x;
    size_t stride = text_spritesheet.width * text_spritesheet.height;
    Sprite sprite = text_spritesheet;
    for (const char* charp = text; *charp != '\0'; ++charp)
    {
        char character = *charp - 32;
        if (character < 0 || character >= 65) continue;

        sprite.data = text_spritesheet.data + character * stride;
        buffer_draw_sprite(buffer2, sprite, xp, y, color);
        xp += sprite.width + 1;
    }
}

void buffer_draw_number(Buffer2* buffer2, const Sprite& number_spritesheet, size_t number, size_t x, size_t y, uint32_t color){
    uint8_t digits[64];
    size_t num_digits = 0;

    size_t current_number = number;
    do
    {
        digits[num_digits++] = current_number % 10;
        current_number = current_number / 10;
    } while (current_number > 0);

    size_t xp = x;
    size_t stride = number_spritesheet.width * number_spritesheet.height;
    Sprite sprite = number_spritesheet;
    for (size_t i = 0; i < num_digits; ++i)
    {
        uint8_t digit = digits[num_digits - i - 1];
        sprite.data = number_spritesheet.data + digit * stride;
        buffer_draw_sprite(buffer2, sprite, xp, y, color);
        xp += sprite.width + 1;
    }
}