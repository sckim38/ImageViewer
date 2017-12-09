#pragma once

class CTextFormat
{
    IDWriteFactory *m_factory = AfxGetD2DState()->GetWriteFactory();
public:
    void CreateTextFormat()
    {
        HRESULT hr;
        hr = m_factory->CreateTextFormat(
            fontFamilyName,                     //WCHAR const* fontFamilyName,
            fontCollection,                     //IDWriteFontCollection* fontCollection,
            fontWeight,                         //DWRITE_FONT_WEIGHT fontWeight,
            fontStyle,                          //DWRITE_FONT_STYLE fontStyle,
            fontStretch,                        //DWRITE_FONT_STRETCH fontStretch,
            fontSize,                           //FLOAT fontSize,
            localeName,                         //WCHAR const* localeName,
            m_pTextFormat.GetAddressOf()        //IDWriteTextFormat** textFormat
            );
        ATLENSURE_SUCCEEDED(hr);
        m_pTextFormat->SetTextAlignment(textAlignment);
        m_pTextFormat->SetParagraphAlignment(paragraphAlignment);
        m_pTextFormat->SetReadingDirection(textReadingDirection);
        m_pTextFormat->SetFlowDirection(textFlowDirection);
    }
    ComPtr<IDWriteTextLayout> CreateTextLayout(LPCWSTR text)
    {
        ComPtr<IDWriteTextLayout> pTextLayout;
        HRESULT hr;
        // Create a text layout using the text format.
        hr = m_factory->CreateTextLayout(
            text,                           // The string to be laid out and formatted.
            (UINT32)wcslen(text),           // The length of the string.
            m_pTextFormat.Get(),            // The text format to apply to the string (contains font information, etc).
            4096.F,                         // The width of the layout box.
            4096.F,                         // The height of the layout box.
            pTextLayout.GetAddressOf()      // The IDWriteTextLayout interface pointer.
            );
        return pTextLayout;
    }
    D2D1_SIZE_F GetTextLayoutSize(LPCWSTR text)
    {
        DWRITE_TEXT_METRICS metrics;
        CreateTextLayout(text)->GetMetrics(&metrics);

        return D2D1::SizeF(
            std::ceil(metrics.widthIncludingTrailingWhitespace),
            std::ceil(metrics.height));
        //return pTextLayout;
    }

    // vertical text : windows 8
    void SetTextLayout()
    {
        m_pTextFormat->SetReadingDirection(textReadingDirection);
        m_pTextFormat->SetFlowDirection(textFlowDirection);
    }
    IDWriteTextFormat *Get()
    {
        return m_pTextFormat.Get();
    }
public:
    WCHAR const* fontFamilyName = L"Verdana";
    IDWriteFontCollection* fontCollection = nullptr;
    DWRITE_FONT_WEIGHT fontWeight = DWRITE_FONT_WEIGHT_NORMAL;
    DWRITE_FONT_STYLE fontStyle = DWRITE_FONT_STYLE_NORMAL;
    DWRITE_FONT_STRETCH fontStretch = DWRITE_FONT_STRETCH_NORMAL;
    FLOAT fontSize = 20.F;
    WCHAR const* localeName = L"";

    DWRITE_TEXT_ALIGNMENT textAlignment = DWRITE_TEXT_ALIGNMENT_LEADING;
    DWRITE_PARAGRAPH_ALIGNMENT paragraphAlignment = DWRITE_PARAGRAPH_ALIGNMENT_NEAR;
    DWRITE_READING_DIRECTION textReadingDirection = DWRITE_READING_DIRECTION_TOP_TO_BOTTOM;
    DWRITE_FLOW_DIRECTION textFlowDirection = DWRITE_FLOW_DIRECTION_RIGHT_TO_LEFT;

    ComPtr<IDWriteTextFormat> m_pTextFormat;
};
