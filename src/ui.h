#ifndef UI_H
#define UI_H

typedef struct ui_color
{
    float r;
    float g;
    float b;
} ui_color_t;

typedef struct ui_vert
{
    struct
    {
        float x;
        float y;
        float z;
    } pos;
    ui_color_t color;

} ui_vert_t;

ui_vert_t *createTestVerts(void);

#endif