#pragma once
#include "canvas/canvas.hpp"
#include <podofo/podofo.h>

namespace horizon {

template <typename T> static T to_um(T x)
{
    return x / 1000;
}

template <typename T> static T to_pt(T x)
{
    return x * .000002834645669291339;
}

class CanvasPDF : public Canvas {
public:
    CanvasPDF(PoDoFo::PdfPainterMM &painter, PoDoFo::PdfFont &font, const class PDFExportSettings &settings);
    void push() override
    {
    }

    void request_push() override;
    bool layer_filter = false;
    int current_layer = 0;
    bool fill = true;
    bool use_layer_colors = false;

private:
    PoDoFo::PdfPainterMM &painter;
    PoDoFo::PdfFont &font;
    const PDFExportSettings &settings;
    const PoDoFo::PdfFontMetrics *metrics;
    void img_line(const Coordi &p0, const Coordi &p1, const uint64_t width, int layer, bool tr) override;
    void img_polygon(const class Polygon &poly, bool tr) override;
    void img_draw_text(const Coordf &p, float size, const std::string &rtext, int angle, bool flip, TextOrigin origin,
                       int layer = 10000, uint64_t width = 0, TextData::Font font = TextData::Font::SIMPLEX,
                       bool center = false, bool mirror = false) override;
    void img_hole(const Hole &hole) override;
    bool pdf_layer_visible(int l) const;
    void draw_polygon(const Polygon &ipoly, bool tr);
    Color get_pdf_layer_color(int layer) const;
};
} // namespace horizon
