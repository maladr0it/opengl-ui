#include <stdlib.h>
#include "utils.h"
#include "ui.h"

static const int INITIAL_CHILDREN_SIZE = 8;
static const int CHILDREN_GROWTH_FACTOR = 2;

typedef struct ui_node
{
    // TODO: this is temporary
    float height;

    ui_color_t color;
    struct
    {
        struct ui_node *members;
        int length;
        int capacity;
    } children;
} ui_node_t;

typedef struct ui_rect
{
    float x;
    float y;
    float height;
    float width;
    ui_color_t color;
    struct
    {
        struct ui_rect *members;
        int length;
        int capacity;
    } children;
} ui_rect_t;

ui_node_t ui_node_create(float height, ui_color_t color)
{
    ui_node_t node;

    node.height = height;
    node.color = color;

    node.children.members = utils_malloc(sizeof(*node.children.members) * INITIAL_CHILDREN_SIZE);
    node.children.capacity = INITIAL_CHILDREN_SIZE;
    node.children.length = 0;

    return node;
}

void ui_node_destroy(ui_node_t *node)
{
    free(node->children.members);
}

void ui_node_appendChild(ui_node_t *node, ui_node_t child)
{
    if (node->children.length >= node->children.capacity)
    {
        int newCapacity = node->children.capacity * CHILDREN_GROWTH_FACTOR;
        node->children.members = utils_realloc(node->children.members, sizeof(*node->children.members) * newCapacity);
        node->children.capacity = newCapacity;
    }
    node->children.members[node->children.length] = child;
    node->children.length += 1;
}

ui_rect_t ui_rect_create(float x, float y, float width, float height, ui_color_t color)
{
    ui_rect_t rect;

    rect.x = x;
    rect.y = y;
    rect.width = width;
    rect.height = height;
    rect.color = color;

    rect.children.members = utils_malloc(sizeof(*rect.children.members) * INITIAL_CHILDREN_SIZE);
    rect.children.capacity = INITIAL_CHILDREN_SIZE;
    rect.children.length = 0;

    return rect;
}

void ui_rect_destroy(ui_rect_t *rect)
{
    free(rect->children.members);
}

void ui_rect_appendChild(ui_rect_t *rect, ui_rect_t child)
{
    if (rect->children.length >= rect->children.capacity)
    {
        int newCapacity = rect->children.capacity * CHILDREN_GROWTH_FACTOR;
        rect->children.members = utils_realloc(rect->children.members, sizeof(*rect->children.members) * newCapacity);
        rect->children.capacity = newCapacity;
    }
    rect->children.members[rect->children.length] = child;
    rect->children.length += 1;
}

void ui_createVerts(ui_vert_t *verts, ui_rect_t rect)
{
    ui_color_t color = rect.color;

    ui_vert_t topLeft = {
        .pos = {
            .x = rect.x,
            .y = rect.y,
            .z = 0.0f,
        },
        .color = color,
    };

    ui_vert_t botLeft = {
        .pos = {
            .x = rect.x,
            .y = rect.y + rect.height,
            .z = 0.0f,
        },
        .color = color,
    };

    ui_vert_t botRight = {
        .pos = {
            .x = rect.x + rect.width,
            .y = rect.y + rect.height,
            .z = 0.0f,
        },
        .color = color,
    };

    ui_vert_t topRight = {
        .pos = {
            .x = rect.x + rect.width,
            .y = rect.y,
            .z = 0.0f,
        },
        .color = color,
    };

    // tri 1
    verts[0] = topLeft;
    verts[1] = botLeft;
    verts[2] = botRight;
    // tri 2
    verts[3] = topLeft;
    verts[4] = botRight;
    verts[5] = topRight;
}

ui_rect_t ui_createLayout(ui_node_t node, ui_rect_t *parent, ui_rect_t *prevSibling)
{
    float x, y, width, height;

    width = parent->width;
    height = node.height;

    if (prevSibling)
    {
        // place it below the prevSibling
        x = prevSibling->x;
        y = prevSibling->y + prevSibling->height;
    }
    else
    {
        x = parent->x;
        y = parent->y;
    }

    ui_rect_t rect = ui_rect_create(x, y, width, height, node.color);

    for (int i = 0; i < node.children.length; ++i)
    {
        ui_rect_t *prevSibling = i > 0 ? &(rect.children.members[i - 1]) : NULL;
        ui_rect_appendChild(&rect, ui_createLayout(node.children.members[i], &rect, prevSibling));
    }

    return rect;
}

ui_vert_t *createTestVerts(void)
{
    ui_color_t WHITE = {.r = 1.0f, .g = 1.0f, .b = 1.0f};
    ui_color_t RED = {.r = 1.0f, .g = 0.0f, .b = 0.0f};
    ui_color_t GREEN = {.r = 0.0f, .g = 1.0f, .b = 0.0f};
    ui_color_t BLUE = {.r = 0.0f, .g = 0.0f, .b = 1.0f};

    ui_vert_t *verts = utils_malloc(sizeof(*verts) * 128);

    ui_rect_t SCREEN_RECT = ui_rect_create(0.0f, 0.0f, 400.0f, 400.0f, WHITE);
    ui_node_t container = ui_node_create(100.0f, GREEN);
    ui_node_t node1 = ui_node_create(50.0f, BLUE);

    ui_node_appendChild(&container, node1);

    ui_rect_t LAYOUT_TREE = ui_createLayout(container, &SCREEN_RECT, NULL);

    ui_createVerts(verts, LAYOUT_TREE);

    return verts;
}
