#pragma once
#include "triangle.hpp"
#include "util/gl_inc.h"
#include <map>
#include <vector>
#include <tuple>
#include "util/vector_pair.hpp"

namespace horizon {
class TriangleRenderer {
    friend class CanvasGL;

public:
    TriangleRenderer(const class CanvasGL &c, const std::map<int, vector_pair<Triangle, TriangleInfo>> &tris);
    void realize();
    void render();
    void push();

private:
    const CanvasGL &ca;
    enum class Type { TRIANGLE, LINE, LINE0, LINE_BUTT, GLYPH, CIRCLE, ARC, ARC0 };
    const std::map<int, vector_pair<Triangle, TriangleInfo>> &triangles;

    struct BatchKey {
        Type type;
        bool highlight;
        bool stencil;

    private:
        auto tie() const
        {
            return std::tie(type, highlight, stencil);
        }

    public:
        bool operator<(const BatchKey &other) const
        {
            return tie() < other.tie();
        }
        bool operator==(const BatchKey &other) const
        {
            return tie() == other.tie();
        }
    };

    struct Span {
        size_t offset;
        size_t count;
    };

    std::map<int, std::map<BatchKey, Span>> layer_offsets;
    size_t n_tris = 0;

    GLuint program_line0;
    GLuint program_line;
    GLuint program_line_butt;
    GLuint program_triangle;
    GLuint program_circle;
    GLuint program_glyph;
    GLuint program_arc;
    GLuint program_arc0;
    GLuint vao;
    GLuint vbo;
    GLuint ubo;
    GLuint ebo;
    GLuint texture_glyph;

    enum class HighlightMode { SKIP, ONLY };
    void render_layer(int layer, HighlightMode highlight_mode, bool ignore_flip = false);
    using Batch = std::vector<decltype(layer_offsets)::mapped_type::value_type>;
    void render_layer_batch(int layer, HighlightMode highlight_mode, bool ignore_flip, const Batch &batch,
                            bool use_stencil, bool stencil_mode);
    void render_annotations(bool top);
    std::array<float, 4> apply_highlight(const class Color &color, HighlightMode mode, int layer) const;
    int stencil = 0;
};
} // namespace horizon
