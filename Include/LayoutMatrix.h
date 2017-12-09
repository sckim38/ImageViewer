#pragma once

inline bool operator==(D2D1_POINT_2F first, D2D1_POINT_2F second)
{
    return (first.x == second.x && first.y == second.y);
}
inline bool operator!=(D2D1_POINT_2F first, D2D1_POINT_2F second)
{
    return !(first == second);
}

inline bool operator==(D2D1_RECT_F first, D2D1_RECT_F second)
{
    return (first.left == second.left
        && first.top == second.top
        && first.right == second.right
        && first.bottom == second.bottom);
}
inline bool operator!=(D2D1_RECT_F first, D2D1_RECT_F second)
{
    return !(first == second);
}


namespace D2D1
{
    template<typename Type, typename Enable = void>
    struct ParamType;

    template<typename Type>
    struct ParamType<Type, typename std::enable_if_t<std::is_same<Type, LPARAM>::value>>
    {
        static auto Get1stParam(Type lParam)->decltype(GET_X_LPARAM(lParam))
        {
            return GET_X_LPARAM(lParam);
        }
        static auto Get2ndParam(Type lParam)->decltype(GET_Y_LPARAM(lParam))
        {
            return GET_Y_LPARAM(lParam);
        }
    };
    template<typename Type>
    struct ParamType<Type, typename std::enable_if_t<is_any<Type, POINT, CPoint, D2D1_POINT_2F>::value>>
    {
        static auto Get1stParam(Type point)->decltype(point.x)
        {
            return point.x;
        }
        static auto Get2ndParam(Type point)->decltype(point.y)
        {
            return point.y;
        }
    };
    template<typename Type>
    struct ParamType<Type, typename std::enable_if_t<is_any<Type, SIZE, CSize>::value>>
    {
        static auto Get1stParam(Type size)->decltype(size.cx)
        {
            return size.cx;
        }
        static auto Get2ndParam(Type size)->decltype(size.cy)
        {
            return size.cy;
        }
    };
    template<typename Type>
    struct ParamType<Type, typename std::enable_if_t<std::is_same<Type, D2D1_SIZE_F>::value>>
    {
        static auto Get1stParam(Type size)->decltype(size.width)
        {
            return size.width;
        }
        static auto Get2ndParam(Type size)->decltype(size.height)
        {
            return size.height;
        }
    };
    template<typename Type>
    struct ParamType<Type, typename std::enable_if_t<is_any<Type, RECT, CRect, D2D1_RECT_F>::value>>
    {
        static auto Get1stParam(Type rect)->decltype(rect.right - rect.left)
        {
            return rect.right - rect.left;
        }
        static auto Get2ndParam(Type rect)->decltype(rect.bottom - rect.top)
        {
            return rect.bottom - rect.top;
        }
    };
    template<typename Type>
    struct ParamType<Type, typename std::enable_if_t<std::is_same<std::decay_t<Type>, ID2D1Bitmap*>::value>>
    {
        static auto Get1stParam(Type pBitmap)->decltype(pBitmap->GetSize().width)
        {
            return pBitmap->GetSize().width;
        }
        static auto Get2ndParam(Type pBitmap)->decltype(pBitmap->GetSize().height)
        {
            return pBitmap->GetSize().height;
        }
    };
    template<typename Type>
    struct ParamType<Type, typename std::enable_if_t<std::is_convertible<std::decay_t<Type>, IWICBitmapSource *>::value>>
    {
        static auto Get1stParam(Type pSource)->UINT
        {
            UINT width, height;
            HRESULT hr = pSource->GetSize(&width, &height);
            ATLENSURE_SUCCEEDED(hr);
            return width;
        }
        static auto Get2ndParam(Type pSource)->UINT
        {
            UINT width, height;
            HRESULT hr = pSource->GetSize(&width, &height);
            ATLENSURE_SUCCEEDED(hr);
            return height;
        }
    };

    template<typename Type>
    inline auto Point2F(Type point)->D2D1_POINT_2F
    {
        return Point2(
            FLOAT(ParamType<Type>::Get1stParam(point)),
            FLOAT(ParamType<Type>::Get2ndParam(point)));
    }
    template<typename Type>
    inline auto SizeF(Type size)->D2D1_SIZE_F
    {
        return Size<FLOAT>(
            FLOAT(ParamType<Type>::Get1stParam(size)),
            FLOAT(ParamType<Type>::Get2ndParam(size)));
    }
    template<typename Type>
    inline auto SizeF(Type point1, Type point2)->D2D1_SIZE_F
    {
        return Size<FLOAT>(
            FLOAT(ParamType<Type>::Get1stParam(point2)) - FLOAT(ParamType<Type>::Get1stParam(point1)),
            FLOAT(ParamType<Type>::Get2ndParam(point2)) - FLOAT(ParamType<Type>::Get2ndParam(point1)));
    }
    inline auto SizeU(IWICBitmapSource *pSource)->D2D1_SIZE_U
    {
        UINT width, height;
        HRESULT hr = pSource->GetSize(&width, &height);
        return Size(width, height);
    }
}

namespace D2D1
{
    template<typename Type, typename Enable = void>
    struct RectangleType;

    template<typename Type>
    struct RectangleType<Type, typename std::enable_if_t<is_any<Type, RECT, CRect, D2D1_RECT_F>::value>>
    {
        static auto GetLeft(Type rect)->decltype(rect.left)
        {
            return rect.left;
        }
        static auto GetTop(Type rect)->decltype(rect.top)
        {
            return rect.top;
        }
        static auto GetRight(Type rect)->decltype(rect.right)
        {
            return rect.right;
        }
        static auto GetBottom(Type rect)->decltype(rect.bottom)
        {
            return rect.bottom;
        }
    };
    template<typename Type>
    struct RectangleType<Type, typename std::enable_if_t<is_any<Type, POINT, D2D1_POINT_2F>::value>>
    {
        static auto GetLeft(Type point)->decltype(point.x)
        {
            return point.x;
        }
        static auto GetTop(Type point)->decltype(point.y)
        {
            return point.y;
        }
        static auto GetRight(Type point)->decltype(point.x)
        {
            return point.x;
        }
        static auto GetBottom(Type point)->decltype(point.y)
        {
            return point.y;
        }
    };
    template<typename Type>
    struct RectangleType<Type, typename std::enable_if_t<std::is_base_of<SIZE, Type>::value>>
    {
        static auto GetLeft(Type size)->decltype(size.cx)
        {
            return decltype(size.cx)(0);
        }
        static auto GetTop(Type size)->decltype(size.cy)
        {
            return decltype(size.cy)(0);
        }
        static auto GetRight(Type size)->decltype(size.cx)
        {
            return size.cx;
        }
        static auto GetBottom(Type size)->decltype(size.cy)
        {
            return size.cy;
        }
    };
    template<typename Type>
    struct RectangleType<Type, typename std::enable_if_t<std::is_same<Type, D2D1_SIZE_F>::value>>
    {
        static auto GetLeft(Type size)->decltype(size.width)
        {
            return decltype(size.width)(0);
        }
        static auto GetTop(Type size)->decltype(size.height)
        {
            return decltype(size.height)(0);
        }
        static auto GetRight(Type size)->decltype(size.width)
        {
            return size.width;
        }
        static auto GetBottom(Type size)->decltype(size.height)
        {
            return size.height;
        }
    };

    inline D2D1_RECT_F RectF(INT l, INT t, INT r, INT b)
    {
        return Rect(FLOAT(l), FLOAT(t), FLOAT(r), FLOAT(b));
    }
    template<typename Type>
    inline D2D1_RECT_F RectF(Type rect)
    {
        return Rect(
            FLOAT(RectangleType<Type>::GetLeft(rect)),
            FLOAT(RectangleType<Type>::GetTop(rect)),
            FLOAT(RectangleType<Type>::GetRight(rect)),
            FLOAT(RectangleType<Type>::GetBottom(rect)));
    }
    template<typename Type>
    inline D2D1_RECT_F RectF(Type point1, Type point2)
    {
        return Rect(
            FLOAT(ParamType<Type>::Get1stParam(point1)),
            FLOAT(ParamType<Type>::Get2ndParam(point1)),
            FLOAT(ParamType<Type>::Get1stParam(point2)),
            FLOAT(ParamType<Type>::Get2ndParam(point2)));
    }
    template<typename Type, typename SizeType>
    inline D2D1_RECT_F RectF(Type point, SizeType size)
    {
        return Rect(
            FLOAT(ParamType<Type>::Get1stParam(point)),
            FLOAT(ParamType<Type>::Get2ndParam(point)),
            FLOAT(ParamType<Type>::Get1stParam(point)) + FLOAT(ParamType<SizeType>::Get1stParam(size)),
            FLOAT(ParamType<Type>::Get2ndParam(point)) + FLOAT(ParamType<SizeType>::Get1stParam(size)));
    }

    inline D2D1_POINT_2F Center(D2D1_RECT_F rect)
    {
        return Point2F(
            (rect.right + rect.left) / 2,
            (rect.bottom + rect.top) / 2);
    }
    inline D2D1_POINT_2F Center(D2D1_POINT_2F point1, D2D1_POINT_2F point2)
    {
        return Point2F(
            (point1.x + point2.x) / 2,
            (point1.y + point2.y) / 2);
    }
    inline D2D1_POINT_2F Center(D2D1_SIZE_F size)
    {
        return Point2F(
            size.width / 2.F,
            size.height / 2.F);
    }
}

namespace Layout
{
    class invert_transform_error : public std::exception
    {
    public:
        invert_transform_error()
            : std::exception("transform exception")
        {}
    };

    const FLOAT c_min_scale = 0.1F;     // min scale factor
    const FLOAT c_max_scale = 5.F;      // max scale factor

    template<typename T>
    struct MatrixInvertor;

    template<>
    struct MatrixInvertor<D2D1_POINT_2F>
    {
        template<typename T>
        static D2D1_POINT_2F InvertTransform(D2D1::Matrix3x2F matrix, T pt)
        {
            if (!matrix.Invert())
                throw invert_transform_error();

            D2D1_POINT_2F point = D2D1::Point2F(pt);
            return matrix.TransformPoint(point);
        }
    };

    template<>
    struct MatrixInvertor<D2D1_RECT_F>
    {
        template<typename T>
        static D2D1_RECT_F InvertTransform(D2D1::Matrix3x2F matrix, T rect)
        {
            if (!matrix.Invert())
                throw invert_transform_error();

            //D2D1_RECT_F rect = D2D1::RectF(rc);
            //D2D1_POINT_2F point1 = D2D1::Point2(rect.left, rect.top);
            //D2D1_POINT_2F point2 = D2D1::Point2(rect.right, rect.bottom);

            D2D1_POINT_2F point1 = matrix.TransformPoint(
                D2D1::Point2(FLOAT(rect.left), FLOAT(rect.top)));
            D2D1_POINT_2F point2 = matrix.TransformPoint(
                D2D1::Point2(FLOAT(rect.right), FLOAT(rect.bottom)));

            return D2D1::RectF(point1, point2);
        }
    };

    class Matrix : public D2D1::Matrix3x2F
    {
    public:
        Matrix()
            : D2D1::Matrix3x2F(D2D1::Matrix3x2F::Identity())
        {
        }
        template<typename T>
        void Offset(T dx, T dy)
        {
            _31 += FLOAT(dx);
            _32 += FLOAT(dy);
        }
        template<typename T>
        void Offset(std::initializer_list<T> l)
        {
            _31 += FLOAT(*l.begin());
            _32 += FLOAT(*(l.begin() + 1));
        }
        template<typename PT>
        bool ScaleAt(FLOAT factor, PT pt) // mouse wheel
        {
            FLOAT new_scale = _11 + factor;

            if (new_scale < c_min_scale || c_max_scale < new_scale)
                return false;

            // first get world position
            auto center = MatrixInvertor<D2D1_POINT_2F>::InvertTransform(*this, pt);
            auto new_matrix = Matrix3x2F::Scale(1 + factor, 1 + factor, center);

            _11 = new_scale;// new_matrix._11;
            _22 = new_scale;// new_matrix._22;
            _31 += new_matrix._31;
            _32 += new_matrix._32;

            // other scale change method
            //auto new_matrix = Matrix3x2F::Scale(factor, factor, center);
            //_11 += new_matrix._11;
            //_22 += new_matrix._22;
            //_31 -= center.x - new_matrix._31;
            //_32 -= center.y - new_matrix._32;

            return true;
        }
        template<typename T>
        void JumpToOrigin(D2D1_POINT_2F center, T rect)
        {
            JumpToOrigin(
                center,
                D2D1::ParamType<T>::Get1stParam(rect),
                D2D1::ParamType<T>::Get2ndParam(rect));
        }
        template<typename T>
        void JumpToOrigin(D2D1_POINT_2F center, T width, T height)
        {
            // get lefttop point
            FLOAT x = center.x - width / 2.F;
            FLOAT y = center.y - height / 2.F;

            // scale
            x *= _11;
            y *= _22;

            // transition
            _31 = -x;
            _32 = -y;
        }
        template<typename ImageType, typename RectType>
        void AdjustScale(ImageType image, RectType rect)
        {
            assert(image);

            D2D1_SIZE_F image_size = D2D1::SizeF(image);
            auto layout_size = D2D1::SizeF(rect);
            auto ratio = D2D1::SizeF(
                layout_size.width / image_size.width,
                layout_size.height / image_size.height
                );

            _11 = _22 = (std::min)(ratio.width, ratio.height);
        }
        template<typename ImageType, typename RectType>
        void AdjustOffset(ImageType image, RectType rect)
        {
            D2D1_SIZE_F image_size = D2D1::SizeF(image);
            D2D1_SIZE_F view_size = D2D1::SizeF(
                image_size.width * _11,
                image_size.height * _22
                );

            auto layout_size = D2D1::SizeF(rect);
            ATLTRACE(_T("AdjustImageLayout:phase0 %.3f, %.3f, %.3f, %.3f, %.3f\n"), _11, _31, image_size.width, layout_size.width, view_size.width);

            D2D1_SIZE_F delta = D2D1::SizeF(
                layout_size.width - view_size.width,
                layout_size.height - view_size.height
                );

            _31 = (std::max)(_31, delta.width);
            _32 = (std::max)(_32, delta.height);

            ATLTRACE(_T("AdjustImageLayout:phase1 %.3f, %.3f, %.3f, %.3f, %.3f\n"), _11, _31, image_size.width, layout_size.width, view_size.width);

            _31 = (std::min)(_31, 0.F);
            _32 = (std::min)(_32, 0.F);

            ATLTRACE(_T("AdjustImageLayout:phase2 %.3f, %.3f, %.3f, %.3f, %.3f\n"), _11, _31, image_size.width, layout_size.width, view_size.width);
        }
    };
    template<typename ImageType, typename RectT>
    inline WICRect CalcClipRect(const D2D1::Matrix3x2F &matrix, ImageType image, RectT window_rect)
    {
        D2D1_SIZE_F image_size = D2D1::SizeF(image);
        D2D1_RECT_F clip_rect = Layout::MatrixInvertor<D2D1_RECT_F>::InvertTransform(matrix, window_rect);
        clip_rect.left = (std::max)(clip_rect.left, 0.F);
        clip_rect.top = (std::max)(clip_rect.top, 0.F);

        clip_rect.right = (std::min)(clip_rect.right, image_size.width);
        clip_rect.right = (std::max)(clip_rect.right, clip_rect.left);
        clip_rect.bottom = (std::min)(clip_rect.bottom, image_size.height);
        clip_rect.bottom = (std::max)(clip_rect.bottom, clip_rect.top);

        WICRect wic_rect;
        wic_rect.X = (INT)clip_rect.left;
        wic_rect.Y = (INT)clip_rect.top;
        wic_rect.Width = (INT)(clip_rect.right - clip_rect.left);
        wic_rect.Height = (INT)(clip_rect.bottom - clip_rect.top);

        return wic_rect;
    }
}


