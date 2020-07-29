#ifndef CVPLOT_H
#define CVPLOT_H

#include <string>
#include <vector>
#include <map>
#include <mutex>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#ifndef byte
typedef unsigned char byte;
#endif

namespace cvplot
{
	class Color
	{
	public:
		Color(byte r, byte g, byte b, byte a = (byte)255)
		{
			r_ = r;
			g_ = g;
			b_ = b;
			a_ = a;
		}

		Color() : Color(0, 0, 0)
		{
			//
		}

		Color& Blend(byte a)
		{
			a_ = a;
			return *this;
		}

		Color& operator=(Color& rhs)
		{
			if (this != &rhs)
			{
				r_ = rhs.r_;
				g_ = rhs.g_;
				b_ = rhs.b_;
				a_ = rhs.a_;
			}
			return *this;
		}

		bool operator==(Color& rhs) const
		{
			return (r_ == rhs.r_ && g_ == rhs.g_ && b_ == rhs.b_ && a_ == rhs.a_);
		}

		bool operator!=(Color& rhs) const
		{
			return (r_ != rhs.r_ || g_ != rhs.g_ || b_ != rhs.b_ || a_ != rhs.a_);
		}

		Color Linear(double intensity)
		{
			if (intensity <= 0 || intensity > 1.0)
			{
				return Color(0, 0, 0, a_);
			}

			int r = r_ * intensity;
			int g = g_ * intensity;
			int b = b_ * intensity;
			int max = std::max({ r,g,b,255 });
			r = r * 255 / max;
			g = g * 255 / max;
			b = b * 255 / max;
			return Color(r, g, b, a_);
		}

		Color Lift(byte v)
		{
			int r = r_ > v ? r_ : v;
			int g = g_ > v ? g_ : v;
			int b = b_ > v ? b_ : v;
			return Color(r, g, b, a_);
		}

		Color Cut(byte v)
		{
			int r = r_ < v ? r_ : v;
			int g = g_ < v ? g_ : v;
			int b = b_ < v ? b_ : v;
			return Color(r, g, b, a_);
		}

		cv::Scalar ToScalar() const
		{
			return cv::Scalar(b_, g_, r_, a_);
		}

	private:
		byte r_, g_, b_, a_;
	};

	namespace chart
	{
		typedef int Type;

		static const Type Bar = 0;
		static const Type Line = 1;
		static const Type Scatter = 2;
		static const Type Elevation = 3;

		static int GetDimension(Type type)
		{
			int dim = 1;

			switch (type)
			{
			case Bar:
				dim = 1;
				break;
			case Line:
			case Scatter:
				dim = 2;
				break;
			case Elevation:
				dim = 3;
				break;
			default:
				break;
			}

			return dim;
		}
	}

	namespace marker
	{
		typedef int Type;

		static const Type None = 0;
		static const Type Cross = 1;
		static const Type Plus = 2;
		static const Type Star = 3;
		static const Type Circle = 4;
		static const Type Square = 5;
		static const Type Diamond = 6;
	}

	namespace color
	{
		static const Color Transparent = Color(255, 255, 255, 0);
		static const Color Red = Color(255, 0, 0);
		static const Color OrangeRed = Color(255, 69, 0);
		static const Color Orange = Color(255, 165, 0);
		static const Color DarkOrange = Color(255, 140, 0);
		static const Color Yellow = Color(255, 255, 0);
		static const Color LawnGreen = Color(124, 252, 0);
		static const Color Green = Color(0, 255, 0);
		static const Color Cyan = Color(0, 255, 255);
		static const Color SkyBlue = Color(135, 206, 235);
		static const Color Blue = Color(0, 0, 255);
		static const Color Black = Color(0, 0, 0);
		static const Color DarkGray = Color(32, 32, 32);
		static const Color Gray = Color(128, 128, 128);
		static const Color LightGray = Color(220, 220, 220);
		static const Color White = Color(255, 255, 255);

		typedef int Gamma;

		namespace gamma
		{
			static byte GAMMA_LUT2P2[] =
			{
				  0,   1,   3,   4,   6,   7,   9,  11,  12,  14,  15,  17,  18,  20,  22,  23,  25,  26,  28,  29,  31,  33,  34,  36,  37,  39,  40,  42,  44,  45,  47,  48,
				 50,  51,  53,  55,  56,  58,  59,  61,  63,  64,  66,  67,  69,  70,  72,  74,  75,  77,  78,  80,  81,  83,  85,  86,  88,  89,  91,  92,  94,  96,  97,  99,
				100, 102, 103, 105, 107, 108, 110, 111, 113, 115, 116, 118, 119, 121, 122, 124, 126, 127, 129, 130, 132, 133, 135, 137, 138, 140, 141, 143, 144, 146, 148, 149,
				151, 152, 154, 155, 157, 159, 160, 162, 163, 165, 166, 168, 170, 171, 173, 174, 176, 178, 179, 181, 182, 184, 185, 187, 189, 190, 192, 193, 195, 196, 198, 200,
				201, 203, 204, 206, 207, 209, 211, 212, 214, 215, 217, 218, 220, 222, 223, 225, 226, 228, 230, 231, 233, 234, 236, 237, 239, 241, 242, 244, 245, 247, 248, 250,
				252, 253, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
				255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
				255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255
			};

			static byte GAMMA_LUT2P0[] =
			{
				  0,   1,   3,   4,   6,   8,   9,  11,  13,  14,  16,  18,  19,  21,  23,  24,  26,  28,  29,  31,  32,  34,  36,  37,  39,  41,  42,  44,  46,  47,  49,  51,
				 52,  54,  56,  57,  59,  61,  62,  64,  65,  67,  69,  70,  72,  74,  75,  77,  79,  80,  82,  84,  85,  87,  89,  90,  92,  93,  95,  97,  98, 100, 102, 103,
				105, 107, 108, 110, 112, 113, 115, 117, 118, 120, 122, 123, 125, 126, 128, 130, 131, 133, 135, 136, 138, 140, 141, 143, 145, 146, 148, 150, 151, 153, 154, 156,
				158, 159, 161, 163, 164, 166, 168, 169, 171, 173, 174, 176, 178, 179, 181, 183, 184, 186, 187, 189, 191, 192, 194, 196, 197, 199, 201, 202, 204, 206, 207, 209,
				211, 212, 214, 215, 217, 219, 220, 222, 224, 225, 227, 229, 230, 232, 234, 235, 237, 239, 240, 242, 244, 245, 247, 248, 250, 252, 253, 255, 255, 255, 255, 255,
				255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
				255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
				255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255
			};

			static byte GAMMA_LUT1P8[] =
			{
				  0,   1,   3,   5,   6,   8,  10,  12,  13,  15,  17,  19,  20,  22,  24,  26,  27,  29,  31,  33,  34,  36,  38,  40,  41,  43,  45,  47,  48,  50,  52,  54,
				 55,  57,  59,  61,  62,  64,  66,  67,  69,  71,  73,  74,  76,  78,  80,  81,  83,  85,  87,  88,  90,  92,  94,  95,  97,  99, 101, 102, 104, 106, 108, 109,
				111, 113, 115, 116, 118, 120, 122, 123, 125, 127, 128, 130, 132, 134, 135, 137, 139, 141, 142, 144, 146, 148, 149, 151, 153, 155, 156, 158, 160, 162, 163, 165,
				167, 169, 170, 172, 174, 176, 177, 179, 181, 183, 184, 186, 188, 189, 191, 193, 195, 196, 198, 200, 202, 203, 205, 207, 209, 210, 212, 214, 216, 217, 219, 221,
				223, 224, 226, 228, 230, 231, 233, 235, 237, 238, 240, 242, 244, 245, 247, 249, 250, 252, 254, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
				255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
				255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
				255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255
			};

			static const Gamma G22 = 1;
			static const Gamma G20 = 2;
			static const Gamma G18 = 3;
		}

		static Color Remap(cv::Scalar color, Gamma gamma = gamma::G20)
		{
			byte b = color[0];
			byte g = color[1];
			byte r = color[2];
			byte a = color[3];

			switch (gamma)
			{
			case gamma::G22:
				r = gamma::GAMMA_LUT2P2[r];
				g = gamma::GAMMA_LUT2P2[g];
				b = gamma::GAMMA_LUT2P2[b];
				break;
			case gamma::G20:
				r = gamma::GAMMA_LUT2P0[r];
				g = gamma::GAMMA_LUT2P0[g];
				b = gamma::GAMMA_LUT2P0[b];
				break;
			case gamma::G18:
				r = gamma::GAMMA_LUT1P8[r];
				g = gamma::GAMMA_LUT1P8[g];
				b = gamma::GAMMA_LUT1P8[b];
				break;
			default:
				break;
			}

			return Color(r, g, b, a);
		}

		static Color Remap(byte r, byte g, byte b, Gamma gamma = gamma::G20)
		{
			switch (gamma)
			{
			case gamma::G22:
				r = gamma::GAMMA_LUT2P2[r];
				g = gamma::GAMMA_LUT2P2[g];
				b = gamma::GAMMA_LUT2P2[b];
				break;
			case gamma::G20:
				r = gamma::GAMMA_LUT2P0[r];
				g = gamma::GAMMA_LUT2P0[g];
				b = gamma::GAMMA_LUT2P0[b];
				break;
			case gamma::G18:
				r = gamma::GAMMA_LUT1P8[r];
				g = gamma::GAMMA_LUT1P8[g];
				b = gamma::GAMMA_LUT1P8[b];
				break;
			default:
				break;
			}

			return Color(r, g, b);
		}

		typedef int Scheme;

		namespace scheme
		{
			static const Scheme Gray = 0;
			static const Scheme Red = 1;
			static const Scheme Green = 2;
			static const Scheme Blue = 3;
		}

		static Color Index(int index, Scheme scheme)
		{
			byte r = 0;
			byte g = 0;
			byte b = 0;

			switch (scheme)
			{
			case scheme::Red:
				r = (byte)(index > 255 ? 255 : index);
				break;
			case scheme::Green:
				g = (byte)(index > 255 ? 255 : index);
				break;
			case scheme::Blue:
				b = (byte)(index > 255 ? 255 : index);
				break;
			default:
				r = (byte)index;
				g = (byte)index;
				b = (byte)index;
				break;
			}

			return Remap(r, g, b);
		}
	}

	namespace util
	{
		static std::mutex gw_mtx__;
		static int gw_current_index__ = 0;

		static int GetUniqueWindowIndex()
		{
			int index;
			gw_mtx__.lock();
			index = ++gw_current_index__;
			gw_mtx__.unlock();
			return index;
		}
	}

	class Series
	{
	public:
		typedef double value_type;

	public:
		Series(const std::string& label, chart::Type chartType, marker::Type markerType = marker::None)
			: label_(label),
			chart_type_(chartType),
			marker_type_(markerType),
			marker_size_({ 4,4 }),
			dimension_(chart::GetDimension(chartType)),
			enable_legend_(true),
			render_color_(color::Blue),
			dirty_(false),
			mins_(dimension_, DBL_MAX),
			maxs_(dimension_, DBL_MIN)
		{
			enable_legend_ = (chartType != chart::Elevation);
		}

		Series& operator=(Series& rhs)
		{
			label_ = rhs.label_;
			chart_type_ = rhs.chart_type_;
			marker_type_ = rhs.marker_type_;
			marker_size_ = rhs.marker_size_;
			dimension_ = rhs.dimension_;
			enable_legend_ = rhs.enable_legend_;
			values_ = rhs.values_;
			mins_ = rhs.mins_;
			maxs_ = rhs.maxs_;
			dirty_ = true;
			return *this;
		}

		Series& SetChartType(chart::Type chartType)
		{
			if (chart_type_ != chartType)
			{
				int dimension = chart::GetDimension(chartType);
				if (dimension_ == dimension)
				{
					chart_type_ = chartType;
					dirty_ = true;
				}
				else
				{
					if (values_.empty())
					{
						chart_type_ = chartType;
						dimension_ = dimension;
						dirty_ = false;
					}
					else
					{
						throw std::exception("dimension not match for the chart type");
					}
				}
			}
			return *this;
		}

		Series& SetMarkerType(marker::Type markerType)
		{
			if (marker_type_ != markerType)
			{
				marker_type_ = markerType;
				dirty_ = true;
			}
			return *this;
		}

		Series& SetMarkerSize(cv::Size size)
		{
			if (marker_size_ != size)
			{
				marker_size_ = size;
				dirty_ = true;
			}
			return *this;
		}

		Series& EnableLegend(bool enable)
		{
			if (enable_legend_ != enable)
			{
				enable_legend_ = enable;
				dirty_ = true;
			}
			return *this;
		}

		Series& SetRenderColor(Color color)
		{
			if (render_color_ != color)
			{
				render_color_ = color;
				dirty_ = true;
			}
			return *this;
		}

		Series& AddValue(value_type value)
		{
			if (dimension_ != 1)
			{
				return *this;
			}

			values_.push_back(value);
			if (maxs_[0] < mins_[0])
			{
				maxs_[0] = value;
				mins_[0] = value;
			}
			else
			{
				if (value > maxs_[0])
				{
					maxs_[0] = value;
				}
				else if (value < mins_[0])
				{
					mins_[0] = value;
				}
			}

			dirty_ = false;
			return *this;
		}

		Series& AddValues(std::vector<value_type> values)
		{
			if (dimension_ < 1)
			{
				return *this;
			}

			auto count = (values.size() / dimension_) * dimension_;
			int j = 0;
			for (int i = 0; i < count; ++i)
			{
				values_.push_back(values[i]);
				j = i % dimension_;
				if (maxs_[j] < mins_[j])
				{
					maxs_[j] = values[i];
					mins_[j] = values[i];
				}
				else
				{
					if (values[i] > maxs_[j])
					{
						maxs_[j] = values[i];
					}
					else if (values[i] < mins_[j])
					{
						mins_[j] = values[i];
					}
				}
			}

			if (count > 0)
			{
				dirty_ = true;
			}
			return *this;
		}

		Series& Clear()
		{
			if (!values_.empty())
			{
				values_.clear();
				for (int i = 0; i < dimension_; ++i)
				{
					maxs_[i] = DBL_MIN;
					mins_[i] = DBL_MAX;
				}
				dirty_ = true;
			}

			return *this;
		}

		int GetDimension() const
		{
			return dimension_;
		}

		int GetCount() const
		{
			return values_.size() / dimension_;
		}

		std::vector<double> GetMax() const
		{
			return std::vector<double>(maxs_);
		}

		std::vector<double> GetMin() const
		{
			return std::vector<double>(mins_);
		}

		std::string GetLabel() const
		{
			return label_;
		}

		chart::Type GetChartType() const
		{
			return chart_type_;
		}

		marker::Type GetMarkerType() const
		{
			return marker_type_;
		}

		cv::Size GetMarkerSize() const
		{
			return marker_size_;
		}

		bool IsLegendEnabled() const
		{
			return enable_legend_;
		}

		Color GetRenderColor() const
		{
			return render_color_;
		}

		bool IsDirty() const
		{
			return dirty_;
		}

		void Draw(cv::Mat& target, int index, int division,
			double x_min, double x_max, double x_delta, double y_min, double y_max, double y_delta)
		{
			if (values_.size() < dimension_)
			{
				dirty_ = false;
				return;
			}

			auto cr = render_color_.ToScalar();

			switch (chart_type_)
			{
			case chart::Bar:
			{
				int x1 = (int)(x_min + x_delta * index + x_delta);
				int x2 = (int)(x1 + x_delta);
				int xd = (int)(x_delta * division);
				int y1 = 0;
				int y2;
				int h = target.rows;
				auto cr1 = render_color_.Cut(90).ToScalar();
				for (auto v : values_)
				{
					y2 = (int)(v > 0 ? v * y_delta : 0.2 * y_delta);
					cv::rectangle(target, { x1,h - y1 }, { x2,h - y2 }, cr, -1);
					char szText[16] = { 0 };
					sprintf_s(szText, "%g", v);
					int fbase;
					auto fface = cv::FONT_HERSHEY_SIMPLEX;
					auto fscale = 0.8;
					auto fsize = cv::getTextSize(szText, fface, fscale, 1, &fbase);
					cv::putText(target, szText, { x1 + ((int)x_delta - fsize.width) / 2,h - y2 - fbase }, fface, fscale, cr1, 1, cv::LINE_AA);
					x1 += xd;
					x2 += xd;
				}
			}
			break;
			case chart::Line:
			case chart::Scatter:
			{
				int h = target.rows;
				int ci = 0;
				std::vector<cv::Point> pts;
				for (int i = 0; i < values_.size(); i += 2)
				{
					int x = (int)((values_[i] - x_min) * x_delta);
					int y = (int)((values_[i + 1] - y_min) * y_delta);
					pts.push_back({ x,h - y });
				}
				int r = (marker_size_.width + marker_size_.height);
				r = (r > 2 && r < 32) ? r : 4;
				
				if (chart_type_ == chart::Line)
				{
					DrawMarkers_(target, pts, marker_type_, render_color_.Cut(64), r, 2);
					cv::polylines(target, pts, false, cr, 1, cv::LINE_AA);
				}
				else if (chart_type_ == chart::Scatter)
				{
					DrawMarkers_(target, pts, marker_type_, render_color_, r, 2);
				}
			}
			break;
			case chart::Elevation:
			{
				int h = target.rows;
				std::vector<cv::Point> pts;
				std::vector<Color> colors;
				auto min = values_[2];
				auto max = values_[2];
				for (int i = 0; i < values_.size(); i += 3)
				{
					int x = (int)((values_[i] - x_min) * x_delta);
					int y = (int)((values_[i + 1] - y_min) * y_delta);
					if (values_[i + 2] > max)
					{
						max = values_[i + 2];
					}
					else if (values_[i + 2] < min)
					{
						min = values_[i + 2];
					}
					pts.push_back({ x,h - y });
				}
				if (max > min)
				{
					double factor = 1.0 / (max - min);
					for (int i = 0; i < values_.size(); i += 3)
					{
						auto z = (values_[i + 2] - min) * factor;
						colors.push_back(render_color_.Linear(z));
					}
				}
				else
				{
					colors.resize(pts.size(), color::Transparent);
				}

				auto block_width = (int)(x_delta);
				auto block_height = (int)(y_delta);
				for (int i = 0; i < pts.size(); ++i)
				{
					cv::Rect rect({ pts[i].x - block_width / 2,pts[i].y - block_height / 2,block_width,block_height });
					cv::rectangle(target, rect, colors[i].ToScalar(), -1);
				}
			}
			break;
			default:
				break;
			}
		}

    private:
		void DrawMarkers_(cv::Mat target, std::vector<cv::Point> pts, marker::Type type, Color color, int size, int thickness)
		{
			cv::Scalar cr = color.ToScalar();
			switch (type)
			{
			case marker::Cross:
			{
				for (auto pt : pts)
				{
					cv::drawMarker(target, pt, cr, cv::MARKER_TILTED_CROSS, size, thickness, cv::LINE_AA);
				}
			}
			break;
			case marker::Plus:
			{
				for (auto pt : pts)
				{
					cv::drawMarker(target, pt, cr, cv::MARKER_CROSS, size, thickness, cv::LINE_AA);
				}
			}
			break;
			case marker::Star:
			{
				for (auto pt : pts)
				{
					cv::drawMarker(target, pt, cr, cv::MARKER_STAR, size, thickness, cv::LINE_AA);
				}
			}
			break;
			case marker::Circle:
			{
				for (auto pt : pts)
				{
					cv::circle(target, pt, size / 2, cr, 2, cv::LINE_8);
				}
			}
			break;
			case marker::Square:
			{
				for (auto pt : pts)
				{
					cv::drawMarker(target, pt, cr, cv::MARKER_SQUARE, size, thickness, cv::LINE_AA);
				}
			}
			break;
			case marker::Diamond:
			{
				for (auto pt : pts)
				{
					cv::drawMarker(target, pt, cr, cv::MARKER_DIAMOND, size, thickness, cv::LINE_AA);
				}
			}
			break;
			default:
				break;
			}
		}

	protected:
		std::string label_;
		chart::Type chart_type_;
		marker::Type marker_type_;
		cv::Size marker_size_;
		int dimension_;
		bool enable_legend_;
		Color render_color_;
		std::vector<value_type> values_;
		std::vector<value_type> maxs_;
		std::vector<value_type> mins_;
		bool dirty_;
	};

	class View
	{
	public:
		View(std::string title, cv::Size size)
			:title_(title),
			size_(size),
			xlabel_(""),
			ylabel_(""),
			background_color_(color::Transparent),
			text_color_(color::Black),
			grid_color_(color::SkyBlue),
			enable_grid_(false),
			dirty_(false),
			horizontal_margin_(50),
			vertical_margin_(80)
		{
			if (size.width > 0 && size.height > 0)
			{
				cv::Size visual_size(size.width + 2 * horizontal_margin_, size.height + 2 * vertical_margin_);
				buffer_ = cv::Mat(visual_size, CV_8UC4, background_color_.ToScalar());
			}
		}

		View() : View("", { 0,0 })
		{
			//
		}

		View& operator=(View& rhs)
		{
			if (this != &rhs && rhs.IsValid())
			{
				title_ = rhs.title_;
				size_ = rhs.size_;
				xlabel_ = rhs.xlabel_;
				ylabel_ = rhs.ylabel_;
				background_color_ = rhs.background_color_;
				text_color_ = rhs.text_color_;
				enable_grid_ = rhs.enable_grid_;
				grid_color_ = rhs.grid_color_;
				horizontal_margin_ = rhs.horizontal_margin_;
				vertical_margin_ = rhs.vertical_margin_;
				buffer_ = rhs.buffer_.clone();
				series_map_ = rhs.series_map_;
				dirty_ = true;
			}
			return *this;
		}

		View& SetTitle(const std::string& title)
		{
			if (title_ != title)
			{
				title_ = title;
				dirty_ = true;
			}
			return *this;
		}

		View& SetSize(cv::Size size)
		{
			if (size_ != size)
			{
				cv::Size visual_size(size.width + 2 * horizontal_margin_, size.height + 2 * vertical_margin_);
				if (buffer_.empty())
				{
					buffer_ = cv::Mat(visual_size, CV_8UC4, color::Transparent.ToScalar());
				}
				else
				{
					auto tmp = buffer_.clone();
					cv::resize(tmp, buffer_, visual_size);
				}
				size_ = size;
				dirty_ = true;
			}
			return *this;
		}

		View& SetTextColor(Color color)
		{
			if (text_color_ != color)
			{
				text_color_ = color;
				dirty_ = true;
			}
			return *this;
		}

		View& SetBackgroundColor(Color color)
		{
			if (background_color_ != color)
			{
				background_color_ = color;
				dirty_ = true;
			}
			return *this;
		}

		View& EnableGrid(bool enable)
		{
			if (enable_grid_ != enable)
			{
				enable_grid_ = enable;
				dirty_ = true;
			}
			return *this;
		}

		View& SetGridColor(Color color)
		{
			if (grid_color_ != color)
			{
				grid_color_ = color;
				dirty_ = true;
			}
			return *this;
		}

		View& SetXLabel(const std::string& xlabel)
		{
			if (xlabel_ != xlabel)
			{
				xlabel_ = xlabel;
				dirty_ = true;
			}
			return *this;
		}

		View& SetYLabel(const std::string& ylabel)
		{
			if (ylabel_ != ylabel)
			{
				ylabel_ = ylabel;
				dirty_ = true;
			}
			return *this;
		}

		bool FindSeries(std::string label)
		{
			auto iter = series_map_.find(label);
			return (iter != series_map_.end());
		}

		View& AddSeries(Series& series)
		{
			if (!series_map_.empty())
			{
				int dim = series_map_.begin()->second.GetDimension();
				if (series.GetDimension() != dim)
				{
					throw std::exception("series dimension confilict");
				}
			}

			auto label = series.GetLabel();
			if (series_map_.find(label) == series_map_.end())
			{
				series_map_.insert({ label, series });
				dirty_ = true;
			}
			return *this;
		}

		View& Remove(const std::string label)
		{
			auto iter = series_map_.find(label);
			if (iter != series_map_.end())
			{
				series_map_.erase(label);
				dirty_ = true;
			}
			return *this;
		}

		View& Clear()
		{
			if (!series_map_.empty())
			{
				series_map_.clear();
				dirty_ = true;
			}
			return *this;
		}

		Series& SelectSeries(std::string label)
		{
			auto iter = series_map_.find(label);
			if (iter == series_map_.end())
			{
				throw std::exception("series not found");
			}
			else
			{
				return iter->second;
			}
		}

		View& Render()
		{
			if (series_map_.empty())
			{
				dirty_ = false;
			}

			if (dirty_)
			{
				//erase view with background
				//buffer_.setTo(background_color_.ToScalar());
				
				//cv::rectangle(buffer_, { 0, 0 }, { size_.width - 1, size_.height - 1 }, color::Black.ToScalar(), 1, cv::LINE_AA);

				//draw y label
				if (!ylabel_.empty())
				{
					auto fface = cv::FONT_HERSHEY_TRIPLEX;
					int fbase;
					auto fsize = cv::getTextSize(ylabel_, fface, 1.0, 2, &fbase);
					int sq_size = size_.width < size_.height ? size_.width : size_.height;
					cv::Rect rect(0, size_.height / 2 + vertical_margin_ - sq_size / 2, sq_size, sq_size);
					auto mat = buffer_(rect);
					cv::Point pt(sq_size / 2 - fsize.width / 2, horizontal_margin_ - fsize.height + 5);
					cv::putText(mat, ylabel_, pt, fface, 1.0, text_color_.ToScalar(), 2, cv::LINE_AA);
					auto rm = cv::getRotationMatrix2D({ sq_size / 2.0f,sq_size / 2.0f }, 90, 1.0);
					cv::warpAffine(mat, mat, rm, { sq_size, sq_size },cv::WARP_FILL_OUTLIERS, cv::BORDER_TRANSPARENT, color::Transparent.ToScalar());
				}

				cv::Rect roi(horizontal_margin_, vertical_margin_, size_.width, size_.height);
				auto target = buffer_(roi);

				double x_min = 0;
				double x_max = 1;
				double x_delta = 1;
				double y_min = 0;
				double y_max = 1;
				double y_delta = 1;

				int dim = series_map_.begin()->second.GetDimension();
				std::vector<double> mins(dim, DBL_MAX);
				std::vector<double> maxs(dim, DBL_MIN);
				for (auto s : series_map_)
				{
					auto mins1 = s.second.GetMin();
					auto maxs1 = s.second.GetMax();
					for (int i = 0; i < dim; ++i)
					{
						if (mins1[i] < mins[i])
						{
							mins[i] = mins1[i];
						}
						if (maxs1[i] > maxs[i])
						{
							maxs[i] = maxs1[i];
						}
					}
				}

				switch (dim)
				{
				case 1:
				{
					int count = 0;
					for (auto s : series_map_)
					{
						auto count1 = s.second.GetCount();
						if (count1 > count)
						{
							count = count1;
						}
					}
					x_min = 0.0;
					x_max = 2.2 * (count + 1);
					y_min = mins[0];
					y_max = maxs[0];
				}
				break;
				case 2:
				case 3:
				{
					x_min = mins[0];
					x_max = maxs[0];
					y_min = mins[1];
					y_max = maxs[1];
				}
				break;
				default:
					break;
				}

				x_delta = 0.8125 * size_.width / (x_max - x_min);
				y_delta = 0.8125 * size_.height / (y_max - y_min);
				x_min = (19.0 * x_min - x_max) / 20.0;
				x_max = (19.0 * x_max - x_min) / 20.0;
				y_min = (19.0 * y_min - y_max) / 20.0;
				y_max = (19.0 * y_max - y_min) / 20.0;

				//draw axis grids
				if (enable_grid_ && dim == 2)
				{
					auto gridLineColor = grid_color_.ToScalar();

					int n_rows = target.rows / y_delta + 1;
					for (auto i = 0; i < n_rows; ++i)
					{
						auto y = (int)(i * y_delta);
						cv::line(target, { 1, buffer_.rows - y }, { buffer_.cols - 1, buffer_.rows - y }, gridLineColor, 1, cv::LINE_4);
					}

					int n_cols = target.cols / x_delta + 1;
					for (auto i = 0; i < n_cols; ++i)
					{
						auto x = (int)(i * x_delta);
						cv::line(target, { x, 1 }, { x, buffer_.rows - 1 }, gridLineColor, 1, cv::LINE_4);
					}
				}
				
				int ci;

				//draw series
				int index = 0;
				int division = series_map_.size() + 1;
				ci = 10;
				for (auto s : series_map_)
				{
					s.second.Draw(target, index, division, x_min, x_max, x_delta, y_min, y_max, y_delta);
					++index;
					ci = (ci + 30) % 256;
					if (s.second.GetDimension() == 3)
					{
						auto render_color = s.second.GetRenderColor();
						cv::Rect rect(size_.width + horizontal_margin_, vertical_margin_, 20, size_.height);
						cv::Mat mat({ 20, 256 }, CV_8UC4);
						for (int i = 0; i < 256; ++i)
						{
							for (int j = 0; j < 20; ++j)
							{
								auto color = color::Remap(render_color.Linear((256 - i) / 256.0).ToScalar());
								mat.at<cv::Vec4b>(i, j) = color.ToScalar();
							}
						}
						cv::Mat tmp(rect.size(), CV_8UC4);
						cv::resize(mat, tmp, { tmp.cols,tmp.rows });
						tmp.copyTo(buffer_(rect));
					}
				}

				//draw legend
				int legend_size = 6;
				int legend_x = size_.width + horizontal_margin_;
				int legend_y = vertical_margin_ + 20;
				ci = 10;
				for (auto s : series_map_)
				{
					if (s.second.IsLegendEnabled())
					{
						auto fface = cv::FONT_HERSHEY_COMPLEX_SMALL;
						int fbase;
						auto fsize = cv::getTextSize(s.first, fface, 1.0, 1, &fbase);
						legend_size = legend_size < fsize.height ? legend_size : fsize.height;
						cv::Point pt(legend_x - 4 * legend_size - fsize.width, legend_y);
						auto cr = s.second.GetRenderColor().ToScalar();
						cv::putText(buffer_, s.first, pt, fface, 1.0, text_color_.ToScalar(), 1, cv::LINE_AA);
						auto chartType = s.second.GetChartType();

						if (s.second.GetChartType() == chart::Line)
						{
							cv::line(buffer_,
								{ legend_x - 2 * legend_size - 4,legend_y - (fsize.height - fbase) / 2 },
								{ legend_x - legend_size,legend_y - (fsize.height - fbase) / 2 },
								cr, 2, cv::LINE_AA);
						}
						else
						{
							switch (s.second.GetMarkerType())
							{
							case marker::Cross:
								cv::drawMarker(buffer_, { legend_x - 2 * legend_size,legend_y - (fsize.height - fbase) / 2 }, cr, cv::MARKER_TILTED_CROSS, legend_size * 2, 2, cv::LINE_AA);
								break;
							case marker::Plus:
								cv::drawMarker(buffer_, { legend_x - 2 * legend_size,legend_y - (fsize.height - fbase) / 2 }, cr, cv::MARKER_CROSS, legend_size * 2, 2, cv::LINE_AA);
								break;
							case marker::Star:
								cv::drawMarker(buffer_, { legend_x - 2 * legend_size,legend_y - (fsize.height - fbase) / 2 }, cr, cv::MARKER_STAR, legend_size * 2, 2, cv::LINE_AA);
								break;
							case marker::Square:
								cv::drawMarker(buffer_, { legend_x - 2 * legend_size,legend_y - (fsize.height - fbase) / 2 }, cr, cv::MARKER_SQUARE, legend_size * 2, 2, cv::LINE_AA);
								break;
							case marker::Diamond:
								cv::drawMarker(buffer_, { legend_x - 2 * legend_size,legend_y - (fsize.height - fbase) / 2 }, cr, cv::MARKER_DIAMOND, legend_size * 2, 2, cv::LINE_AA);
								break;
							case marker::Circle:
								cv::circle(buffer_, { legend_x - 2 * legend_size,legend_y - (fsize.height - fbase) / 2 }, legend_size, cr, 2, cv::LINE_AA);
								break;
							default:
								cv::circle(buffer_, { legend_x - 2 * legend_size,legend_y - (fsize.height - fbase) / 2 }, legend_size, cr, -1, cv::LINE_AA);
								break;
							}
						}
						ci = (ci + 30) % 256;
						legend_y += 2 * fsize.height;
					}
				}

				//draw title
				if (!title_.empty())
				{
					auto fface = cv::FONT_HERSHEY_SIMPLEX;
					int fbase;
					auto fsize = cv::getTextSize(title_, fface, 1.5, 2, &fbase);
					cv::Point pt(size_.width / 2 + horizontal_margin_ - fsize.width / 2, vertical_margin_ > fsize.height ? vertical_margin_ - fsize.height : 5);
					cv::putText(buffer_, title_, pt, fface, 1.5, color::Black.ToScalar(), 2, cv::LINE_AA);
				}

				//draw x label
				if (!xlabel_.empty())
				{
					auto fface = cv::FONT_HERSHEY_TRIPLEX;
					int fbase;
					auto fsize = cv::getTextSize(xlabel_, fface, 1.0, 2, &fbase);
					cv::Point pt(size_.width / 2 + horizontal_margin_ - fsize.width / 2, size_.height + vertical_margin_ + fsize.height + 5);
					cv::putText(buffer_, xlabel_, pt, fface, 1.0, text_color_.ToScalar(), 2, cv::LINE_AA);
				}

				dirty_ = false;
			}

			return *this;
		}

		void Invalidate()
		{
			dirty_ = true;
		}

		std::string GetTitle() const
		{
			return title_;
		}

		cv::Size GetSize() const
		{
			return size_;
		}

		bool IsValid() const
		{
			return !buffer_.empty();
		}

		bool IsDirty() const
		{
			return dirty_;
		}

		cv::Mat GetBuffer() const
		{
			if (buffer_.empty())
			{
				return cv::Mat();
			}

			return buffer_.clone();
		}

	private:
		static double CalcSnap(double value)
		{
			auto v1 = pow(10, floor(log10(value)));
			auto v2 = pow(10, floor(log10(value / 2))) * 2;
			auto v3 = pow(10, floor(log10(value / 5))) * 5;
			return std::max({ v1,v2,v3 });
		}

	protected:
		std::string title_;
		cv::Size size_;
		std::string xlabel_;
		std::string ylabel_;
		Color background_color_;
		Color text_color_;
		Color grid_color_;
		bool enable_grid_;
		bool dirty_;
		cv::Mat buffer_;
		int horizontal_margin_;
		int vertical_margin_;
		std::map<std::string, Series> series_map_;
	};

	class Figure
	{
	public:
		Figure()
			: horizontal_count_(1),
			vertical_count_(1),
			horizontal_margin_(20),
			vertical_margin_(20),
			figure_size_({ 800,800 }),
			buffer_(800, 800, CV_8UC4, color::Gray.ToScalar())
		{
			int index = util::GetUniqueWindowIndex();
			char sz[16] = { 0 };
			sprintf_s(sz, "fig-%09d", index);
			figure_name_ = sz;
			views_.push_back(View());
		}

		~Figure()
		{
			cv::destroyWindow(figure_name_);
		}

		Figure& SetSize(cv::Size size)
		{
			if (figure_size_ != size)
			{
				figure_size_ = size;
				auto tmp = cv::Mat(figure_size_, CV_8UC4, Color(200, 200, 200, 128).ToScalar());
				cv::resize(buffer_, tmp, figure_size_, 0, 0, cv::INTER_NEAREST);
				buffer_ = tmp;
			}
			return *this;
		}

		Figure& SetLayout(int rows, int cols)
		{
			int count_ = horizontal_count_ * vertical_count_;
			int count = rows * cols;
			if (count_ != count)
			{
				views_.resize(count, View("", { 800,800 }));
			}

			horizontal_count_ = cols;
			vertical_count_ = rows;
			return *this;
		}

		Figure& SetView(View& view, int hpos, int vpos)
		{
			int index = (hpos - 1) * horizontal_count_ + vpos - 1;
			if (index < 0 || index >= views_.size())
			{
				throw std::out_of_range("view index out of range");
			}
			views_[index] = view;
			return *this;
		}

		View& SelectView(int hpos, int vpos)
		{
			int index = (hpos - 1) * horizontal_count_ + vpos - 1;
			if (index < 0 || index >= views_.size())
			{
				throw std::out_of_range("view index out of range");
			}
			else
			{
				return views_[index];
			}
		}

		void Show(bool waitKey = true)
		{
			Render_();
			cv::namedWindow(figure_name_, cv::WINDOW_AUTOSIZE);
			const int SCW = 1920;
			const int SCH = 800;
			int x = (SCW - figure_size_.width) / 2;
			int y = (SCH - figure_size_.height) / 2;
			x = (x > 0 && x < 200) ? x : 10;
			y = (y > 0 && y < 200) ? y : 10;
			cv::moveWindow(figure_name_, x, y);
			cv::imshow(figure_name_, buffer_);
			if (waitKey)
			{
				cv::waitKey();
			}
		}

		void Close()
		{
			cv::destroyWindow(figure_name_);
		}

		void Save(const std::string& filename, bool saveSubViews = false)
		{
			Render_();
			try
			{
				cv::imwrite(filename, buffer_);
				if (saveSubViews && !render_results_.empty())
				{
					auto index = filename.find_last_of('.');
					std::string prefix = filename.substr(0, index);
					std::string ext = filename.substr(index);
					for (auto result : render_results_)
					{
						auto filename_1 = prefix + "[" + result.first + "]" + ext;
						cv::imwrite(filename_1, result.second);
					}
				}
			}
			catch (...)
			{

			}
		}

	private:
		void Render_()
		{
			bool dirty_ = false;
			for (auto view : views_)
			{
				if (view.IsDirty())
				{
					dirty_ = true;
					break;
				}
			}

			if (dirty_)
			{
				auto res_width = figure_size_.width - (horizontal_count_ + 1) * horizontal_margin_;
				auto res_height = figure_size_.height - (vertical_count_ + 1) * vertical_margin_;
				cv::Size size =
				{
					res_width / horizontal_count_,
					res_height / vertical_count_
				};

				cv::Rect roi(horizontal_margin_, 0, size.width, size.height);

				int index = 0;
				for (int i = 0; i < horizontal_count_; ++i)
				{
					roi.y = vertical_margin_;
					for (int j = 0; j < vertical_count_; ++j)
					{
						auto vbuf = views_[index].Render().GetBuffer();
						if (!vbuf.empty())
						{
							char sz[8] = { 0 };
							sprintf_s(sz, "%02d-%02d", j + 1, i + 1);
							if (render_results_.find(sz) == render_results_.end())
							{
								render_results_.insert({ sz, vbuf });
							}
							else
							{
								render_results_[sz] = vbuf;
							}
							auto m = buffer_(roi);
							if (vbuf.rows != size.height || vbuf.cols != size.width)
							{
								cv::Mat tmp(size, CV_8UC4);
								cv::resize(vbuf, tmp, size, cv::INTER_NEAREST);
								tmp.copyTo(m);
								tmp.release();
							}
							else
							{
								vbuf.copyTo(m);
							}
						}
						++index;
						roi.y += (size.height + vertical_margin_);
					}
					roi.x += (size.width + horizontal_margin_);
				}

				dirty_ = false;
			}
		}

	private:
		std::vector<View> views_;
		std::map<std::string, cv::Mat> render_results_;
		std::string figure_name_;
		cv::Size figure_size_;
		int horizontal_count_;
		int vertical_count_;
		int horizontal_margin_;
		int vertical_margin_;
		cv::Mat buffer_;
	};
}

#endif  // CVPLOT_H
