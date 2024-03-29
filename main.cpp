#include <dicomhero6/codecFactory.h>
#include <dicomhero6/dataSet.h>
#include <dicomhero6/dicomDictionary.h>
#include <dicomhero6/dicomDir.h>
#include <dicomhero6/dicomDirEntry.h>
#include <dicomhero6/personName.h>
#include <dicomhero6/tag.h>
#include <dicomhero6/tagId.h>

#include <cassert>
#include <charconv>
#include <concepts>
#include <filesystem>
#include <iostream>
#include <sstream>

using namespace dicomhero;

auto pad(size_t level)
    -> std::string
{ return std::string( level*4, ' ' ); }

/* We currently need these tag data types:
AE +
CS +
DA +
IS +
LO +
OB +
PN +
SH +
SQ +
TM +
UI +
UL +
US +

AS +
DS +
DT +
FD +
FL +
LT +
SL +
SS +
*/

template <typename H, typename T>
using IndexedGetterMethod =
    T (H::*)(size_t) const;

template <typename H>
using TagReadingHandlerMethod =
    IndexedGetterMethod<Tag, H>;

template <typename G, typename T, std::derived_from<G> H>
auto tag_values(const Tag& tag,
                TagReadingHandlerMethod<H> getReadingHandler,
                IndexedGetterMethod<G,T> getValue)
    -> std::vector< std::vector<T> >
{
    auto result = std::vector< std::vector<T> >{};

    auto nbuf = tag.getBuffersCount();

    for (size_t ibuf=0; ibuf<nbuf; ++ibuf)
    {
        auto item = std::vector<T>{};

        auto h = (tag.*getReadingHandler)(ibuf);
        auto nval = h.getSize();

        for (size_t ival=0; ival<nval; ++ival)
            item.push_back((h.*getValue)(ival));

        result.push_back(std::move(item));
    }
    return result;
}

template <typename T>
auto flatten(const std::vector<std::vector<T>>& values)
    -> std::vector<T>
{
    auto result = std::vector<T>{};
    for (const auto& item : values)
        for (const auto& value : item)
            result.push_back(value);
    return result;
}

struct DefaultFormatter
{
    template <typename T>
    auto operator()(std::ostream& s, const T& value) const
        -> void
    { s << value; }
};

template<typename Seq, typename FormatItem = DefaultFormatter>
auto maybe_format_seq(std::ostream& s,
                      const Seq& seq,
                      std::string_view open,
                      std::string_view close,
                      std::string_view delim,
                      FormatItem fmt = {})
{
    if (seq.size() != 1)
        s << open;

    size_t index = 0;
    for (const auto& item : seq)
    {
        if (index != 0)
            s << delim;
        if (index == 10)
        {
            s << "...";
            break;
        }
        fmt(s, item);
        ++index;
    }

    if (seq.size() != 1)
        s << close;
}

template <typename ItemFormatter = DefaultFormatter>
struct MaybeSeqFormatter
{
    std::string_view open;
    std::string_view close;
    std::string_view delim;
    ItemFormatter fmt;

    MaybeSeqFormatter(std::string_view open,
                      std::string_view close,
                      std::string_view delim,
                      ItemFormatter fmt = {})
        : open(open)
        , close(close)
        , delim(delim)
        , fmt{ std::move(fmt) }
    {}

    template <typename T>
    auto operator()(std::ostream& s, const T& value) const
        -> void
    { maybe_format_seq(s, value, open, close, delim, fmt); }
};

template <typename G, typename T, std::derived_from<G> H>
auto format_tag_values(const Tag& tag,
                       std::string_view vr_name,
                       TagReadingHandlerMethod<H> getReadingHandler,
                       IndexedGetterMethod<G,T> getValue)
    -> std::string
{
    std::ostringstream s;
    s << vr_name << ": ";

    auto values = tag_values(tag, getReadingHandler, getValue);

    maybe_format_seq(
        s, values, "[", "]", ", ", MaybeSeqFormatter{"{", "}", ", "});

    return s.str();
}

constexpr auto TagReadBase =
    &Tag::getReadingDataHandler;

constexpr auto TagReadNumeric =
    &Tag::getReadingDataHandlerNumeric;

constexpr auto TagReadI32 =
    &ReadingDataHandlerNumeric::getInt32;

constexpr auto TagReadU32 =
    &ReadingDataHandlerNumeric::getUint32;

constexpr auto TagReadI16 =
    &ReadingDataHandlerNumeric::getInt16;

constexpr auto TagReadU16 =
    &ReadingDataHandlerNumeric::getUint16;

constexpr auto TagReadF32 =
    &ReadingDataHandlerNumeric::getFloat;

constexpr auto TagReadF64 =
    &ReadingDataHandlerNumeric::getDouble;

constexpr auto TagReadStr =
    &ReadingDataHandler::getString;

auto tag_to_string(const TagId& tag_id, const DataSet& ds)
    -> std::string
{
    auto tag = ds.getTag(tag_id);

    switch (tag.getDataType())
    {
    case tagVR_t::AE: // Application Entity
        return format_tag_values(tag, "AE", TagReadBase, TagReadStr);
    case tagVR_t::AS: // Age String
        return format_tag_values(tag, "AS", TagReadBase, TagReadStr);
    case tagVR_t::AT: // Attribute Tag
        return "TODO: AT";
    case tagVR_t::CS: // Code String
        return format_tag_values(tag, "CS", TagReadBase, TagReadStr);
    case tagVR_t::DA: // Date
        return format_tag_values(tag, "DA", TagReadBase, TagReadStr);
    case tagVR_t::DS: // Decimal String
        return format_tag_values(tag, "DS", TagReadBase, TagReadStr);
    case tagVR_t::DT: // Date Time
        return format_tag_values(tag, "DT", TagReadBase, TagReadStr);
    case tagVR_t::FL: // Floating Point Single
        return format_tag_values(tag, "FL", TagReadNumeric, TagReadF32);
    case tagVR_t::FD: // Floating Point Double
        return format_tag_values(tag, "FD", TagReadNumeric, TagReadF64);
    case tagVR_t::IS: // Integer String
        return format_tag_values(tag, "IS", TagReadBase, TagReadStr);
    case tagVR_t::LO: // Long String
        return format_tag_values(tag, "LO", TagReadBase, TagReadStr);
    case tagVR_t::LT: // Long Text
        return format_tag_values(tag, "LT", TagReadBase, TagReadStr);
    case tagVR_t::OB: // Other Byte String
        return format_tag_values(tag, "OB", TagReadBase, TagReadStr);
        // return "TODO: OB";
    case tagVR_t::SB: // Non standard. Used internally for signed bytes
        return "TODO: SB";
    case tagVR_t::OD: // Other Double String
        return "TODO: OD";
    case tagVR_t::OF: // Other Float String
        return "TODO: OF";
    case tagVR_t::OL: // Other Long String
        return "TODO: OL";
    case tagVR_t::OV: // Other Very long
        return "TODO: OV";
    case tagVR_t::OW: // Other Word String
        return "TODO: OW";
    case tagVR_t::PN: // Person Name
        return "PN: " +
               ds.getPersonName(tag_id, 0).getAlphabeticRepresentation();
    case tagVR_t::SH: // Short String
        return format_tag_values(tag, "SH", TagReadBase, TagReadStr);
    case tagVR_t::SL: // Signed Long
        return format_tag_values(tag, "SL", TagReadNumeric, TagReadI32);
    case tagVR_t::SQ: // Sequence of Items
        return format_tag_values(tag, "SQ", TagReadBase, TagReadStr);
    case tagVR_t::SS: // Signed Short
        return format_tag_values(tag, "SS", TagReadNumeric, TagReadI16);
    case tagVR_t::ST: // Short Text
        return "TODO: ST";
    case tagVR_t::SV: // Signed Very Long
        return "TODO: SV";
    case tagVR_t::TM: // Time
        return format_tag_values(tag, "TM", TagReadBase, TagReadStr);
    case tagVR_t::UC: // Unlimited characters
        return "TODO: UC";
    case tagVR_t::UI: // Unique Identifier
        return format_tag_values(tag, "UI", TagReadBase, TagReadStr);
    case tagVR_t::UL: // Unsigned Long
        return format_tag_values(tag, "UL", TagReadNumeric, TagReadU32);
    case tagVR_t::UN: // Unknown
        return "TODO: UN";
    case tagVR_t::UR: // Unified Resource Identifier
        return "TODO: UR";
    case tagVR_t::US: // Unsigned Short
        return format_tag_values(tag, "US", TagReadNumeric, TagReadU16);
    case tagVR_t::UT: // Unlimited Text
        return "TODO: UT";
    case tagVR_t::UV: // Unsigned Very Long
        return "TODO: UV";
    }
    throw std::invalid_argument("Unexpected tag type");
}

auto to_string(dicomhero::bitDepth_t v)
    -> std::string
{
    switch (v)
    {
        case dicomhero::bitDepth_t::depthU8: return "depthU8";
        case dicomhero::bitDepth_t::depthS8: return "depthS8";
        case dicomhero::bitDepth_t::depthU16: return "depthU16";
        case dicomhero::bitDepth_t::depthS16: return "depthS16";
        case dicomhero::bitDepth_t::depthU32: return "depthU32";
        case dicomhero::bitDepth_t::depthS32: return "depthS32";
    }
}

template <typename T> struct Type_Tag final{};
template <typename T> static inline constexpr auto Type = Type_Tag<T>{};

class AsciiImage final
{
public:
    AsciiImage(size_t max_width,
               size_t max_height,
               const dicomhero::Image& image)
    {
        // Get the color space
        std::string colorSpace = image.getColorSpace();

        // Get the size in pixels, as size_t
        size_t width = image.getWidth();
        size_t height = image.getHeight();

        if (width*max_height <= max_width*height)
        {
            if (height <= max_height)
            {
                height_ = height;
                width_ = width;
            }
            else
            {
                height_ = max_height;
                width_ = width * max_height / height;
            }
        }
        else
        {
            if (width <= max_width)
            {
                width_ = width;
                height_ = height;
            }
            else
            {
                width_ = max_width;
                height_ = height * max_width / width;
            }
        }

        image_ = std::string( 2*width_*height_, '.' );

        auto nch = image.getChannelsNumber();
        auto depth = image.getDepth();
        auto hi_bit = image.getHighBit();

        auto dh = image.getReadingDataHandler();
        size_t data_length;
        auto* data = dh.data(&data_length);
        size_t bytesPerValue = dh.getUnitSize();
        auto is_signed = dh.isSigned();

        struct PX
        {
            int64_t acc{0};
            int64_t count{0};
        };
        std::vector<PX> px(width_ * height_);

        auto acc_pixels = [&]<typename ValueType>(Type_Tag<ValueType>)
        {
            assert(sizeof(ValueType) == bytesPerValue);
            assert(std::is_signed_v<ValueType> == is_signed);

            for (size_t y=0; y<height; ++y)
            {
                auto yy = y * height_ / height;
                auto *dst_row = px.data() + width_*yy;
                for(size_t x=0; x<width; ++x)
                {
                    auto xx = x * width_ / width;
                    auto* dst = dst_row + xx;

                    dst->acc += *reinterpret_cast<const ValueType*>(data);
                    data += sizeof(ValueType);

                    ++dst->count;
                }
            }
        };

        switch (depth)
        {
        case dicomhero::bitDepth_t::depthU8:  acc_pixels(Type<uint8_t >); break;
        case dicomhero::bitDepth_t::depthS8:  acc_pixels(Type<int8_t  >); break;
        case dicomhero::bitDepth_t::depthU16: acc_pixels(Type<uint16_t>); break;
        case dicomhero::bitDepth_t::depthS16: acc_pixels(Type<int16_t >); break;
        case dicomhero::bitDepth_t::depthU32: acc_pixels(Type<uint32_t>); break;
        case dicomhero::bitDepth_t::depthS32: acc_pixels(Type<int32_t >); break;
        }

        // TODO: Multiple channels

        for (auto& p : px)
            p.acc /= p.count;

        auto pmin = px[0].acc;
        auto pmax = pmin;
        for (auto& p : px)
        {
            pmin = std::min(pmin, p.acc);
            pmax = std::max(pmax, p.acc);
        }
        if (pmax == pmin)
            ++pmax;

        constexpr char brightness_table[] =
            " `.-':_,^=;><+!rc*/z?sLTv)J7(|Fi{C}fI31tlu[neoZ5Yxjya]2ESwqkP6h9d4"
            "VpOGbUAKXHm8RD#$Bg0MNWQ%&@";
        auto brightness_table_size = sizeof(brightness_table) - 1;

        auto* dst = image_.data();
        for (auto& p : px)
        {
            auto brightness =
                (p.acc - pmin) * (brightness_table_size-1) / (pmax-pmin);
            auto v = brightness_table[brightness];
            *dst++ = v;
            *dst++ = v;
        }
    }

    auto rows() const
        -> std::vector<std::string_view>
    {
        auto result = std::vector<std::string_view>{};
        result.reserve(height_);
        const auto* d = image_.data();
        auto _2w = 2*width_;
        for(size_t row=0; row<height_; ++row, d+=_2w)
            result.emplace_back(d, d+_2w);
        return result;
    }

private:
    std::string image_;
    size_t width_;
    size_t height_;
};

auto preview_image(const dicomhero::Image& image, size_t level)
    -> void
{
    auto pimg = pad(level) + "> ";

    std::cout
        << pimg << "size: " << image.getWidth()
                << " x " << image.getHeight() << '\n'
        << pimg << "color space: " << image.getColorSpace() << '\n'
        << pimg << "channels: " << image.getChannelsNumber() << '\n'
        << pimg << "depth: " << to_string(image.getDepth()) << '\n'
        << pimg << "hi_bit: " << image.getHighBit() << '\n'
        ;

    auto ascii_img = AsciiImage{ 50, 30, image };

    for (const auto& row : ascii_img.rows())
        std::cout << pimg << row << std::endl;
}

auto describeDataSet(const std::filesystem::path& path,
                     const DataSet& dataSet,
                     bool has_images,
                     size_t level)
    -> void
{
    auto p = pad(level);

    auto refers_to_image = false;

    constexpr size_t CharBufSize = 100;
    char buf[CharBufSize];
    auto hex = [&buf](auto x) -> std::string_view
    {
        auto res = std::to_chars(buf, buf+CharBufSize, x, 16);
        assert(res.ec == std::error_code{});
        return {buf, res.ptr};
    };

    DicomDictionary dict;
    auto tagDescription = [&](const TagId& id)
        -> std::string
    {
        try
        {
            return dict.getTagDescription(id);
        }

        catch(std::exception&)
        {
            return "UNKNOWN TAG";
        }
    };

    for (const auto& tag_id : dataSet.getTags())
    {
        auto gr = tag_id.getGroupId();
        auto id = tag_id.getTagId();
        auto str = tag_to_string(tag_id, dataSet);

        std::cout
            << p
            << hex(gr) << "_" << hex(id)
            << ": " << str
            << " - " << tagDescription(tag_id)
            << std::endl;

        auto tagId_v =
            static_cast<tagId_t>((uint32_t{gr} << 16) | id);

        if (tagId_v == tagId_t::DirectoryRecordType_0004_1430 && str == "CS: IMAGE")
            refers_to_image = true;
    }

    if (refers_to_image)
    {
        auto tag = dataSet.getTag(TagId(tagId_t::ReferencedFileID_0004_1500));
        auto path_items = flatten(tag_values(tag, TagReadBase, TagReadStr));

        auto img_path = path;
        for (const auto& item : path_items)
            img_path /= item;

        std::cout << p << "REFERS TO IMAGE at path " << img_path << std::endl;

        auto img_ds = CodecFactory::load(img_path);
        describeDataSet(img_path, img_ds, true, level+1);
    }

    if (has_images)
    {
        auto frame_count =
            dataSet.getUint32(TagId{tagId_t::NumberOfFrames_0028_0008}, 0, 1);

        for (uint32_t iimg=0; iimg<frame_count; ++iimg)
        {
            auto pimg = p + "> ";
            try
            {
                // Retrieve the first image (index = 0)
                dicomhero::Image image(dataSet.getImageApplyModalityTransform(iimg));
                // dicomhero::Image image(dataSet.getImage(0));

                std::cout << p << "Image " << iimg << std::endl;
                preview_image(image, level);
            }
            catch(std::exception& e)
            {
                std::cout
                    << p << "Image index: " << iimg
                    << " - getImage failure: " << e.what() << std::endl;
            }

        }
        std::cout << p << "Image count: " << frame_count << std::endl;
    }

    std::cout << "----" << std::endl;
}

auto describeDatasetTree(const std::filesystem::path& path,
                         DicomDirEntry root,
                         bool has_images,
                         size_t level)
{
    describeDataSet(path, root.getEntryDataSet(), has_images, level);

    if (!root.hasChildren())
        return;

    auto e = root.getFirstChildEntry();
    size_t index = 0;
    while(true)
    {
        describeDatasetTree(path, e, false, level+1);

        if (!e.hasNextEntry())
            break;

        if (++index == 3)
        {
            std::cout << pad(level+1) << "..." << std::endl;
            break;
        }

        auto tmp = e;
        e.~DicomDirEntry();
        new (&e)DicomDirEntry(tmp.getNextEntry());
    }
}


int main(int argc, char*argv [])
{
    if (argc != 2)
    {
        auto exe_path = std::filesystem::path(argv[0]);
        auto exe_name = exe_path.filename();
        std::cerr << "Usage: " << exe_name.string()
                  << " path_to_dir_containing_DICOMDIR_file"
                  << std::endl;
        return EXIT_FAILURE;
    }
    auto path =
        std::filesystem::path(argv[1]);

    DataSet dataSet = CodecFactory::load(path / "DICOMDIR");

    describeDataSet(path, dataSet, false, 0);

    DicomDir dd(dataSet);
    describeDatasetTree(path, dd.getFirstRootEntry(), false, 1);
    return EXIT_SUCCESS;
}
